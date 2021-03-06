type evalResult = {. "errorMessage": Js.nullable(string)};

[@bs.val] external eval : string => unit = "";

let evalJs = code =>
  try (
    {
      eval(code);
      %bs.obj
      {errorMessage: Js.Nullable.null};
    }
  ) {
  | Js.Exn.Error(e) =>
    %bs.obj
    {errorMessage: Js.Nullable.fromOption(Js.Exn.message(e))}
  };
