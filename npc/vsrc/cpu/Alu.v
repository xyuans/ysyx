module Alu #(WIDTH = 4) (
  input [WIDTH-1:0] a,
  input [WIDTH-1:0] b,
  output reg [WIDTH-1:0] y
);

  assign y = a + b;
  
endmodule
