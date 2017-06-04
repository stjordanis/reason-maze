
module type Config = {
  let batch: int;
};

module Default: Config = {
  let batch = 10;
};

module Draw (Board: Shared.Board) (Generator: Shared.Generator) (DrawConfig: DrawShared.Config) (Config: Config) => {
  module Draw = DrawShared.Draw Board DrawConfig;

  let draw ctx bsize csize => {
    let adjacency = Board.adjacency_list bsize;
    let state = Generator.init (Board.Shape.vertex_count bsize) adjacency;
    Canvas.Ctx.setLineCap ctx "round";

    let rec loop state => {
      let walls = Walls.walls_remaining adjacency (Generator.State.traveled state);

      Draw.draw ctx bsize csize walls (Generator.State.traveled state) (Generator.State.current state) (Generator.State.next state);

      switch (Generator.State.current state) {
        | [] => ()
        | _ => {
          let tmp = ref state;
          for _ in 0 to (Config.batch - 1) {
            tmp := (Generator.step !tmp);
          };
          Window.setTimeout (fun () => loop !tmp) 100 |> ignore
        }
      }
    };
    loop state
  };
};