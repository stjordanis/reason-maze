
let iof = int_of_float;

let module F (Board: SimpleBoard.T) (Gen: Generator.T) => {
  let module Man = PaintingManager.F Board Gen;
  let module Presenter = Presenter.F Board Gen;
  let module Show' = Show.F Board Gen;

  let show_paint ctx (width, height) state => {
    Canvas.Ctx.setStrokeWidth ctx 1.0;
    Canvas.Ctx.setLineCap ctx "round";
    Canvas.Ctx.clearRect ctx 0.0 0.0 width height;
    /* TODO might be nice to do something sophisticated with corners... */

    let (w, h) = state.PaintingManager.State.outsize;
    let xm = (width -. w) /. 2.0;
    let ym = (height -. h) /. 2.0;
    /*let (xm, ym) = (0.0, 0.0);*/

    Array.iter (Presenter.draw_shape ctx (xm, ym) 10) (Man.paint_shapes state);
    Canvas.Ctx.setStrokeStyle ctx "#aaa";
    /*List.iter (Presenter.draw_wall ctx (xm, ym)) (Man.paint_walls state);*/
  };

  let listen_to_canvas = [%bs.raw {|function(canvas, fn) {
    canvas.addEventListener('mousemove', evt => {
      if (evt.button === 0 && evt.buttons === 1) {
        fn([evt.clientX, evt.clientY])
      }
    })
  }|}];

  let make_button = [%bs.raw {|function(text, fn) {
    var button = document.createElement('button')
    button.textContent = text;
    document.body.appendChild(button)
    button.addEventListener('click', fn)
  }|}];

  let paint () => {
    Random.self_init();

    let canvas_size = (1000.0, 1000.0);
    let (width, height) = canvas_size;
    let min_margin = 50.0;

    let canvas = Canvas.createOnBody (iof width) (iof height);
    let ctx = Canvas.getContext canvas;

    let with_margins = (width -. min_margin, height -. min_margin);
    let paint_state = Man.paint_init with_margins 80;  

    show_paint ctx canvas_size paint_state;

    let (w, h) = paint_state.PaintingManager.State.outsize;
    let xm = (width -. w) /. 2.0;
    let ym = (height -. h) /. 2.0;

    let pstate = ref paint_state;
    listen_to_canvas canvas (fun (x, y) => {
      pstate := Man.toggle_point !pstate (x -. xm, y -. ym);
      show_paint ctx canvas_size !pstate;
    });
    make_button "Go" (fun () => {
      let state = Man.realize_state !pstate;
      /* TODO need to make it work with unconnected sections */
      Show'.animate ctx 5 canvas_size state;
    });
  };
};