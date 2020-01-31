module Bindings = struct
  external c_check_for_other_wm: unit -> int = "caml_check_for_other_wm"

  let x_set_error_handler () : int =
     c_check_for_other_wm ()
end
