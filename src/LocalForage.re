external getItem_ : string => Js.Promise.t (Js.Nullable.t 'a) =
  "getItem" [@@bs.module "localforage"];

external setItem_ : string => 'a => Js.Promise.t 'a = "setItem" [@@bs.module "localforage"];

external removeItem_ : string => Js.Promise.t unit = "removeItem" [@@bs.module "localforage"];

external clear_ : unit => Js.Promise.t unit = "clear" [@@bs.module "localforage"];

let getItem key => {
  let promise = getItem_ key;
  let vow = Vow.Result.wrap promise (fun () => Js.log ("Error: getItem " ^ key));
  Vow.Result.map (fun nullable => Vow.Result.return (Js.Nullable.to_opt nullable)) vow
};

let setItem key value => {
  let promise = setItem_ key value;
  Vow.Result.wrap promise (fun () => Js.log ("Error: setItem " ^ key))
};

let removeItem key => {
  let promise = removeItem_ key;
  Vow.Result.wrap promise (fun () => Js.log ("Error: removeItem " ^ key))
};

let clear () => {
  let promise = clear_ ();
  Vow.Result.wrap promise (fun () => Js.log "Error: clear")
};
