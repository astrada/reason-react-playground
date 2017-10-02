type evalResult = Js.t {. errorMessage : Js.Nullable.t string};

external eval : string => unit = "" [@@bs.val];

let evalJs code =>
  try {
    eval code;
    {"errorMessage": Js.Nullable.null}
  } {
  | Js.Exn.Error e => {"errorMessage": Js.Nullable.from_opt (Js.Exn.message e)}
  };
