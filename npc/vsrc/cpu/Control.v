/* verilator lint_off WIDTHTRUNC */
import "DPI-C" function int ebreak();

module Control(
  input [6:0] op,
  output reg_write,
  output [1:0] imm_src,
  output alu_src,
  output [1:0] wd_src,
  output [1:0] pc_src
);
  reg [7:0] all_out;
  always @(*) begin 
    case(op)
      // reg_write, imm_src, alu_src, wd_src, pc_src
      // imm_src: 00-i  01-u  10-j
      // wd_src: 00-alu_resualt  01-pc+4  10-imm  11-imm+pc
      // pc_src: 00-pc+4  01-imm+pc 10-alu_resualt(imm+rst1)
      7'b0110111: all_out = 8'b1_01_x_10_00;  // lui,   u
      7'b0010111: all_out = 8'b1_01_x_11_00;  // auipc, u
      7'b1101111: all_out = 8'b1_10_x_01_01;  // jal,   j
      7'b1100111: all_out = 8'b1_00_1_01_10;  // jalr,  i
      7'b0010011: all_out = 8'b1_00_1_00_00;  // addi,  i
      7'b0110011: all_out = 8'b1_xx_0_00_00;  // add,   r
      7'b1110011: all_out = ebreak();
      default: all_out = 8'b0_xx_x_xx_xx;
    endcase
  end
  assign {reg_write, imm_src, alu_src, wd_src, pc_src} = all_out;
endmodule
