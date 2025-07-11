/* verilator lint_off WIDTHTRUNC */
module Control(
  input [6:2] op,
  input [14:12] funct3,
  input funct7,  // inst[30]
  output reg_write,
  output [2:0] imm_src,
  output alu_src,
  output [3:0] alu_ctr,
  output mem_write,
  output [2:0] mem_op,
  output [2:0] wd_src,
  output [2:0] branch
  
);
  reg [18:0] all_out;
  assign {reg_write, imm_src, alu_src, alu_ctr, mem_write, mem_op, wd_src, branch} = all_out;
  always @(*) begin 
    casez({funct7, funct3, op})
      // reg_write, imm_src, alu_src, alu_ctr, mem_write, wd_src, pc_src
      // imm_src: 000-i  001-s  010-b  011-u  100-j
      // alu_src: 0-rd2 1-imm
      // alu_ctr:
      //          alu_ctr[3]-sub/add or arith/logical
      //          alu_ctr[2:0]:000-sum 001-lshift 010-slt 011-sltu
      //                       100-xor 101-rshift 110-or  111-and or unsign/sign
      // 
      // mem_write:
      // mem_op: 000-1byte,sign  001-2bs  010-4b  100-1bu  101-2bu
      // wd_src: 000-alu_resualt  001-pc+4  010-imm  011-imm+pc 1xx:mem
      // branch: 000-非跳转指令 001-jal, 010-jalr, 
      //         100-beq, 101-bnq, 110-blt, 111-bge
      // 
      9'b?_???_01101: all_out = 19'b1_011_x_xxxx_0_xxx_010_000;  // lui,   u
      9'b?_???_00101: all_out = 19'b1_011_x_xxxx_0_xxx_011_000;  // auipc, u
      9'b?_???_11011: all_out = 19'b1_100_x_xxxx_0_xxx_001_001;  // jal,   j
      9'b?_000_11001: all_out = 19'b1_000_1_0000_0_xxx_000_010;  // jalr,  i
      9'b?_000_11000: all_out = 19'b0_010_0_1000_0_xxx_xxx_100;  // beq,   b
      9'b?_001_11000: all_out = 19'b0_010_0_1xx0_0_xxx_xxx_101;  // bnq,   b
      9'b?_100_11000: all_out = 19'b0_010_0_1xx0_0_xxx_xxx_110;  // blt,   b
      9'b?_101_11000: all_out = 19'b0_010_0_1xx0_0_xxx_xxx_111;  // bge,   b
      9'b?_110_11000: all_out = 19'b0_010_0_1xx1_0_xxx_xxx_110;  // bltu,  b
      9'b?_111_11000: all_out = 19'b0_010_0_1xx1_0_xxx_xxx_111;  // bgeu,  b
      9'b?_000_00000: all_out = 19'b1_000_1_0000_0_000_1xx_000;  // lb,    i
      9'b?_001_00000: all_out = 19'b1_000_1_0000_0_001_1xx_000;  // lh,    i
      9'b?_010_00000: all_out = 19'b1_000_1_0000_0_010_1xx_000;  // lw,    i
      9'b?_100_00000: all_out = 19'b1_000_1_0000_0_100_1xx_000;  // lbu,   i
      9'b?_101_00000: all_out = 19'b1_000_1_0000_0_101_1xx_000;  // lhu,   i
      9'b?_000_01000: all_out = 19'b0_001_1_0000_1_000_xxx_000;  // sb,    s
      9'b?_001_01000: all_out = 19'b0_001_1_0000_1_001_xxx_000;  // sh,    s
      9'b?_010_01000: all_out = 19'b0_001_1_0000_1_010_xxx_000;  // sw,    s
      9'b?_000_00100: all_out = 19'b1_000_1_0000_0_xxx_000_000;  // addi,  i
      9'b?_010_00100: all_out = 19'b1_000_1_1010_0_xxx_000_000;  // slti,  i
      9'b?_011_00100: all_out = 19'b1_000_1_1011_0_xxx_000_000;  // sltiu, i
      9'b?_100_00100: all_out = 19'b1_000_1_x100_0_xxx_000_000;  // xori,  i
      9'b?_110_00100: all_out = 19'b1_000_1_x110_0_xxx_000_000;  // ori,   i
      9'b?_111_00100: all_out = 19'b1_000_1_x111_0_xxx_000_000;  // andi,  i
      9'b0_001_00100: all_out = 19'b1_000_1_0001_0_xxx_000_000;  // slli,  i
      9'b0_101_00100: all_out = 19'b1_000_1_0101_0_xxx_000_000;  // srli,  i
      9'b1_101_00100: all_out = 19'b1_000_1_1101_0_xxx_000_000;  // srai,  i
      9'b0_000_01100: all_out = 19'b1_xxx_0_0000_0_xxx_000_000;  // add,   r
      9'b1_000_01100: all_out = 19'b1_xxx_0_1000_0_xxx_000_000;  // sub,   r
      9'b0_001_01100: all_out = 19'b1_xxx_0_0001_0_xxx_000_000;  // sll,   r
      9'b0_010_01100: all_out = 19'b1_xxx_0_1010_0_xxx_000_000;  // slt,   r
      9'b0_011_01100: all_out = 19'b1_xxx_0_1011_0_xxx_000_000;  // sltu,  r
      9'b0_100_01100: all_out = 19'b1_xxx_0_x100_0_xxx_000_000;  // xor,   r
      9'b0_101_01100: all_out = 19'b1_xxx_0_0101_0_xxx_000_000;  // srl,   r
      9'b1_101_01100: all_out = 19'b1_xxx_0_1101_0_xxx_000_000;  // sra,   r
      9'b0_110_01100: all_out = 19'b1_xxx_0_x100_0_xxx_000_000;  // or,    r
      9'b0_111_01100: all_out = 19'b1_xxx_0_x111_0_xxx_000_000;  // and,   r

      default: all_out = 8'b0_xx_x_xx_xx;
    endcase
  end

endmodule
