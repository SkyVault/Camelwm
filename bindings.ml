module Signal = struct
  external n_sigchld : int -> unit = "caml_sigchld"

  let sigchld i = i |> n_sigchld
end

module X = struct
    type display
    type window = int

    external n_select_input : display -> window -> int -> unit = "caml_x11_SelectInput"
    external n_open_display : string -> display = "caml_x11_OpenDisplay"

    external default_screen : display -> int = "caml_x11_DefaultScreen"
    external display_width : display -> int -> int = "caml_x11_DisplayWidth"
    external display_height  : display -> int -> int = "caml_x11_DisplayHeight"
    external root_window : display -> int -> window = "caml_x11_RootWindow"

    let select_input (d : display) (w : window) (m : int) =
        n_select_input d w m

    let open_display (name : string option) : display option =
      match name with
      | Some s -> Some (n_open_display s)
      | None -> None
end

module Bindings = struct
    external n_check_for_other_wm : X.display -> int = "caml_check_for_other_wm"

    let check_for_other_wm display : bool =
      n_check_for_other_wm display |> (fun n -> n = 0)
end
