module Common = struct
  type status = | Success | Failed

  let int_to_status = function
    | 0 -> Failed
    | 1 -> Success
    | _ -> Success
end
