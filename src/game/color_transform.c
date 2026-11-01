#include "../types.h"
#include "color.h"

HsvT *Color_RgbToHsl(HsvT *hsl_out, u8 red, u8 green, u8 blue, u8 channel_limit) {
    u8 channels[3];
    HsvT hsl_value;
    u8 channel_value;
    s32 next_channel_index;
    s32 remaining_channel_index;
    s32 channel_index;
    s32 maximum_index;
    s32 minimum_index;
    s32 maximum_channel;
    s32 minimum_channel;
    s32 twice_channel_limit;
    s32 channel_sum;
    s32 lightness;
    s32 channel_delta_work;
    s32 scaled_channel_sum;
    s32 hue_work;

    s32 hue_result;
    maximum_index = 0;
    minimum_index = 0;
    channel_index = 1;
    channels[0] = red;
    channels[1] = green;
    channels[2] = blue;

    do {
        channel_value = channels[channel_index];
        if (channel_value > channels[maximum_index]) {
            maximum_index = channel_index;
        }
        if (channel_value < channels[minimum_index]) {
            minimum_index = channel_index;
        }
        channel_index++;
    } while (channel_index < 3);

    maximum_channel = channels[maximum_index];
    minimum_channel = channels[minimum_index];
    channel_sum = maximum_channel + minimum_channel;
    scaled_channel_sum = channel_sum << COLOR_FIXED_SHIFT;
    twice_channel_limit = (channel_limit & 0xFF) * 2;
    lightness = scaled_channel_sum / twice_channel_limit;

    hsl_value.v = 0;
    hsl_value.h = 0;
    channel_delta_work = minimum_channel;
    channel_delta_work = maximum_channel - channel_delta_work;
    hsl_value.s = lightness;

    if (channel_delta_work != 0) {
        if ((u32)(lightness & 0xFFFF) < COLOR_FIXED_HALF + 1) {
            hsl_value.v = (channel_delta_work << COLOR_FIXED_SHIFT) / channel_sum;
        } else {
            hsl_value.v = (channel_delta_work << COLOR_FIXED_SHIFT) / (twice_channel_limit - channel_sum);
        }
        next_channel_index = (maximum_index + 1) % 3;
        remaining_channel_index = (maximum_index + 2) % 3;
        hue_work = (channels[next_channel_index] - channels[remaining_channel_index]) << COLOR_FIXED_SHIFT;
        hue_result = (maximum_index << (COLOR_FIXED_SHIFT + 1)) + hue_work / channel_delta_work;
        hue_work = hue_result;
        hsl_value.h = hue_work;
        if (hue_work < 0) {
            hsl_value.h = hue_work + COLOR_HUE_FULL_TURN;
        }
    }

    *hsl_out = hsl_value;
    return hsl_out;
}

Color *Color_HslToRgb(Color *rgb_out, s32 hue, u16 lightness, u16 saturation, u8 channel_limit)
{
    u32 channels_fixed[3];
    Color rgb_value;
    s32 hue_sector;
    s32 maximum_index;
    s32 minimum_index;
    s32 interpolated_index;
    s32 edge_weight;
    s32 edge_position;

    hue_sector = (hue / COLOR_FIXED_ONE + 1) % COLOR_HUE_SECTOR_COUNT;
    maximum_index = hue_sector / 2;
    if (hue_sector & 1) {
        interpolated_index = (maximum_index + 1) % 3;
        minimum_index = (maximum_index + 2) % 3;
    } else {
        minimum_index = (maximum_index + 1) % 3;
        interpolated_index = (maximum_index + 2) % 3;
    }

    if (lightness < COLOR_FIXED_HALF + 1) {
        channels_fixed[minimum_index] = (lightness * (COLOR_FIXED_ONE - saturation) / COLOR_FIXED_ONE) * channel_limit;
        channels_fixed[maximum_index] = (lightness << 1) * channel_limit - channels_fixed[minimum_index];
    } else {
        channels_fixed[maximum_index] = (lightness * (COLOR_FIXED_ONE - saturation) / COLOR_FIXED_ONE + saturation) * channel_limit;
        channels_fixed[minimum_index] = (lightness << 1) * channel_limit - channels_fixed[maximum_index];
    }

    edge_position = (hue + COLOR_FIXED_ONE) % COLOR_HUE_FULL_TURN -
        maximum_index * (2 * COLOR_FIXED_ONE);
    edge_weight = edge_position - COLOR_FIXED_ONE;
    if (edge_weight < 0) {
        edge_weight = COLOR_FIXED_ONE - edge_position;
    }
    channels_fixed[interpolated_index] = (channels_fixed[maximum_index] - channels_fixed[minimum_index]) * edge_weight / COLOR_FIXED_ONE + channels_fixed[minimum_index];

    channels_fixed[0] = (channels_fixed[0] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    channels_fixed[1] = (channels_fixed[1] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    channels_fixed[2] = (channels_fixed[2] + COLOR_FIXED_HALF) / COLOR_FIXED_ONE;
    rgb_value.r = channels_fixed[0] < 0x100 ? channels_fixed[0] : 0xFF;
    rgb_value.g = channels_fixed[1] < 0x100 ? channels_fixed[1] : 0xFF;
    rgb_value.b = channels_fixed[2] < 0x100 ? channels_fixed[2] : 0xFF;
    *rgb_out = rgb_value;
    return rgb_out;
}
s32 Color_TintBgr555Pixel(u16 pixel, s32 tint_control, u16 saturation_scale)
{
    Color packing_rgb;
    HsvT hsl_state;
    Color output_rgb;
    u8 channel_limit;
    s32 hue_selector;
    u8 forced_hue;
    s32 invert_mask;
    s32 grayscale_request;

    if (pixel == 0) {
        return 0;
    }

    invert_mask = tint_control & COLOR_TINT_INVERT;
    hue_selector = tint_control & COLOR_TINT_HUE_MASK;
    grayscale_request = ((u8)hue_selector == COLOR_TINT_GRAYSCALE);
    Color_RgbToHsl(
        &hsl_state,
        pixel & COLOR_BGR555_CHANNEL_MASK,
        (pixel >> COLOR_BGR555_GREEN_SHIFT) & COLOR_BGR555_CHANNEL_MASK,
        (pixel >> COLOR_BGR555_BLUE_SHIFT) & COLOR_BGR555_CHANNEL_MASK,
        COLOR_BGR555_CHANNEL_MASK);

    channel_limit = COLOR_BGR555_CHANNEL_MASK;
    if ((u8)hue_selector < COLOR_TINT_KEEP_HUE) {
        forced_hue = hue_selector;
        if (invert_mask) {
            forced_hue = (forced_hue + COLOR_HUE_SECTOR_COUNT / 2) % COLOR_HUE_SECTOR_COUNT;
        }
        hsl_state.h = forced_hue << COLOR_FIXED_SHIFT;
    }

    if (grayscale_request) {
        hsl_state.v = 0;
    } else {
        hsl_state.v = (hsl_state.v * saturation_scale) / COLOR_FIXED_ONE;
    }

    Color_HslToRgb(&output_rgb, hsl_state.h, hsl_state.s, hsl_state.v, COLOR_BGR555_CHANNEL_MASK);

    if (invert_mask) {
        output_rgb.r = channel_limit - output_rgb.r;
        output_rgb.g = channel_limit - output_rgb.g;
        output_rgb.b = channel_limit - output_rgb.b;
    }

    output_rgb.r = output_rgb.r ? output_rgb.r : 1;
    output_rgb.g = output_rgb.g ? output_rgb.g : 1;
    output_rgb.b = output_rgb.b ? output_rgb.b : 1;
    packing_rgb = output_rgb;
    return (packing_rgb.r & COLOR_BGR555_CHANNEL_MASK) |
           ((packing_rgb.g & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_GREEN_SHIFT) |
           ((packing_rgb.b & COLOR_BGR555_CHANNEL_MASK) << COLOR_BGR555_BLUE_SHIFT) |
           (pixel & COLOR_BGR555_STP_MASK);
}

s32 Color_HslToBgr555Pixel(s32 hue, u32 lightness_word, u32 saturation_word)
{
    Color output_rgb;

    ((void (*)(Color *, s32, u32, u32, s32))Color_HslToRgb)(
        &output_rgb, hue, lightness_word & 0xFFFF, saturation_word & 0xFFFF, COLOR_BGR555_CHANNEL_MASK
    );
    return (output_rgb.r & COLOR_BGR555_CHANNEL_MASK) |
           ((output_rgb.g & COLOR_BGR555_CHANNEL_MASK)
            << COLOR_BGR555_GREEN_SHIFT) |
           ((output_rgb.b & COLOR_BGR555_CHANNEL_MASK)
            << COLOR_BGR555_BLUE_SHIFT);
}

Color *Color_TintRgb(
    Color *rgb_out, u8 red, u8 green, u8 blue, s32 tint_control, u16 saturation_scale, u8 channel_limit
)
{
    HsvT hsl_state;
    Color output_rgb;
    s32 hue_selector;
    s32 invert_mask;
    s32 grayscale_request;
    u8 forced_hue;

    invert_mask = tint_control & COLOR_TINT_INVERT;
    hue_selector = tint_control & COLOR_TINT_HUE_MASK;
    grayscale_request = ((u8)hue_selector == COLOR_TINT_GRAYSCALE);

    ((void (*)(HsvT *, u8, u8, u8, u8))Color_RgbToHsl)(&hsl_state, red, green, blue, channel_limit);

    if ((u8)hue_selector < COLOR_TINT_KEEP_HUE) {
        forced_hue = hue_selector;
        if (invert_mask) {
            forced_hue = (forced_hue + COLOR_HUE_SECTOR_COUNT / 2) %
                COLOR_HUE_SECTOR_COUNT;
        }
        hsl_state.h = forced_hue << COLOR_FIXED_SHIFT;
    }

    if (grayscale_request) {
        hsl_state.v = 0;
    } else {
        hsl_state.v = hsl_state.v * saturation_scale / COLOR_FIXED_ONE;
    }

    ((void (*)(Color *, s32, u32, u32, s32))Color_HslToRgb)(&output_rgb, hsl_state.h, hsl_state.s, hsl_state.v, channel_limit);

    if (invert_mask) {
        output_rgb.r = channel_limit - output_rgb.r;
        output_rgb.g = channel_limit - output_rgb.g;
        output_rgb.b = channel_limit - output_rgb.b;
    }

    output_rgb.r = output_rgb.r ? output_rgb.r : 1;
    output_rgb.g = output_rgb.g ? output_rgb.g : 1;
    output_rgb.b = output_rgb.b ? output_rgb.b : 1;

    *rgb_out = output_rgb;
    return rgb_out;
}
