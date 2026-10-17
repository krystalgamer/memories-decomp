# Paired resource constructor return

`func_800291E0` returns its allocated-second `DisplayObject`, which it also
publishes as the resource record's primary object and links from the first
object. The local already has that actual type. Its public return now keeps
`struct DisplayObject *` through a forward declaration instead of discarding
the type through `u8 *`; neither the pool API nor the three scalar arguments
changes.

All observed game callers already cast to `DisplayObject *`. Password keeps
its existing explicit `PasswordCardPreviewView *` cast. No caller or overlay
implementation needs changing, and deliberate narrower/configuration views
are not replaced. No function-pointer consumers were found.

The grouped constructor changes only its declared return and final
redundant byte cast. The selected object, returned address, publication
order, signed-card/default-coordinate logic, exact compiler profile and
all matching instructions remain unchanged; this adds no allocation or
null-safety guarantee.

The ritual native fixture's stub uses the corrected declaration while
returning the exact same object address. Its events, expectations and
regression mutants stay unchanged. Standalone supported-compiler probes
accept the exact function-pointer return, reject byte/opaque returns,
and retain explicit partial/byte and ordinary generic caller views.
