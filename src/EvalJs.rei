type evalResult = Js.t {. errorMessage : Js.Nullable.t string};

let evalJs: string => evalResult;
