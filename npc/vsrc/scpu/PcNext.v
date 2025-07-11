module PcNext(
  input branch [2:0],
  input zero,
  input less,
  output reg [1:0] pc_src
);
  wire [4:0] input_all;
  assign input_all = {branch, zero, less};
  always @(*) begin
    casez (input_all)
      5'b000_?_?: pc_src = 2'b00;  // 非跳转指令,pc+4
      5'b001_?_?: pc_src = 2'b01;  // jal, pc+imm
      5'b010_?_?: pc_src = 2'b10;  // jalr, rs1+imm
      5'b100_0_?: pc_src = 2'b00;  // beq, == 
      5'b100_1_?: pc_src = 2'b01;  // beq, ==
      5'b101_0_?: pc_src = 2'b01;  // bnq, !=
      5'b101_1_?: pc_src = 2'b00;  // bnq, !=
      5'b110_?_0: pc_src = 2'b00;  // blt, <
      5'b110_?_1: pc_src = 2'b01;  // blt, <
      5'b111_?_0: pc_src = 2'b01;  // bge, >=
      5'b111_?_1: pc_src = 2'b00;  // bge, >=
    endcase
  end
endmodule
