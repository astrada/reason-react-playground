let optionMap fn option =>
  switch option {
  | Some value => Some (fn value)
  | None => None
  };

external lodashDebounce : 'a => Js.Nullable.t float => 'a = "lodash.debounce" [@@bs.module];

type options = {
  leading: bool,
  maxWait: float,
  trailing: bool
};

let defaultOptions = {leading: false, maxWait: 0., trailing: true};

let debounce func ::wait=0.0 => lodashDebounce func (Js.Nullable.return wait);
