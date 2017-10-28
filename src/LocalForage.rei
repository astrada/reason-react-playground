let getItem: string => Vow.Result.t (option 'a) unit Vow.handled;

let setItem: string => 'a => Vow.Result.t 'a unit Vow.handled;

let removeItem: string => Vow.Result.t unit unit Vow.handled;

let clear: unit => Vow.Result.t unit unit Vow.handled;
