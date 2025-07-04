module RegFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input clk,
  input we,
  input [ADDR_WIDTH-1:0] ra1,
  input [ADDR_WIDTH-1:0] ra2,
  input [ADDR_WIDTH-1:0] wa,
  input [DATA_WIDTH-1:0] wd,
  output reg [DATA_WIDTH-1:0] rd1,
  output reg [DATA_WIDTH-1:0] rd2 
);
  reg [DATA_WIDTH-1:0] regs [2**ADDR_WIDTH-1:0];

  always @(posedge clk) begin
    if(we) regs[wa] <= wd;
  end
  
  assign rd1 = (ra1 != 0) ? regs[ra1] : 0;
  assign rd2 = (ra2 != 0) ? regs[ra2] : 0;
endmodule
