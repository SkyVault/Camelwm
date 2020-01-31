open Printf
open Bindings
(* open Common *)

let check_for_other_wm () =
    Bindings.x_set_error_handler

let () =
    printf "Starting Camel WM";
    check_for_other_wm () |> ignore
                             
