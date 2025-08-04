module PcNext (
    inout csr_pc,
    input [2:0] branch,
    input zero,
    input less,
    output reg [1:0] pc_src
);
  wire [4:0] input_all;
  assign input_all = {csr_pc, branch, zero, less};
  always @(*) begin
    casez (input_all)
      6'b0_000_?_?: pc_src = 2'b00;  // 非跳转指令,pc+4
      6'b0_001_?_?: pc_src = 2'b01;  // jal, pc+imm
      6'b0_010_?_?: pc_src = 2'b10;  // jalr, rs1+imm
      6'b0_100_0_?: pc_src = 2'b00;  // beq, ==
      6'b0_100_1_?: pc_src = 2'b01;  // beq, ==
      6'b0_101_0_?: pc_src = 2'b01;  // bnq, !=
      6'b0_101_1_?: pc_src = 2'b00;  // bnq, !=
      6'b0_110_?_0: pc_src = 2'b00;  // blt, <
      6'b0_110_?_1: pc_src = 2'b01;  // blt, <
      6'b0_111_?_0: pc_src = 2'b01;  // bge, >=
      6'b0_111_?_1: pc_src = 2'b00;  // bge, >=
      6'b1_???_?_?: pc_src = 2'b11;  // mepc或mtvec,选择用csr输出的pc源
      default: pc_src = 2'b00;
    endcase
  end
endmodule
