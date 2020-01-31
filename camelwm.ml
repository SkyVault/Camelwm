open Printf
open Bindings
(* open Common *)

type camel_t =
  { the_display : X.display option
  ; the_window : X.window option
  ; the_screen : int
  ; display_size : int * int
  ; the_root_window : X.window
  }

let setup (wm : camel_t) : camel_t =
  let
    wm = { wm with the_screen =
                     match wm.the_display with
                     | Some d -> X.default_screen d
                     | None -> 0 };
  in
    match (wm.the_display, wm.the_screen) with
    | (Some d, s) -> 
       let width = X.display_width d s in
       let height = X.display_height d s in

       let root = X.root_window d s in

       { wm with display_size = (width, height)
               ; the_root_window = root }
    | _ ->
        wm

let () =
  printf "Starting Camel WM";

  { the_display = X.open_display None
  ; the_window = None
  ; the_screen = 0
  ; display_size = (0, 0)
  ; the_root_window = 0 }
  |> (fun wm ->
    match wm.the_display with
    | Some d ->
       d  |> Bindings.check_for_other_wm |> ignore;
       wm |> setup |> ignore;

       ()
    | None -> ())
  |> ignore
                             
