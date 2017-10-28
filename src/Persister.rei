type code =
  | Null
  | Reason(string)
  | OCamlJsx(string);

let loadPersistedState: unit => Vow.Result.t(code, unit, Vow.handled);

let saveState: code => unit;
