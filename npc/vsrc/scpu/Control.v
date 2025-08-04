/* verilator lint_off WIDTHTRUNC */
module Control (
    input [6:2] op,
    input [14:12] funct3,
    input [30:29] funct7,  // inst[30:29]
    output reg_write,
    output [2:0] imm_src,
    output alu_src,
    output [3:0] alu_ctr,
    output mem_write,
    output [2:0] mem_op,
    output [2:0] wd_src,
    output [2:0] branch,
    output [1:0] csr_ctr
);


  reg [20:0] all_out;
  assign {reg_write, imm_src, alu_src, alu_ctr, mem_write, mem_op, wd_src, branch, csr} = all_out;
  always @(*) begin
    casez ({
      funct7, funct3, op
    })
      // reg_write, imm_src, alu_src, alu_ctr, mem_write, mem_op, wd_src, branch, csr
      // reg_write:1-write 0-no write
      // imm_src: 000-i  001-s  010-b  011-u  100-j
      // alu_src: 0-rd2 1-imm
      // alu_ctr:
      //          alu_ctr[3]-sub/add or arith/logical
      //          alu_ctr[2:0]:000-sum 001-lshift 010-slt 011-sltu
      //                       100-xor 101-rshift 110-or  111-and or unsign/sign
      //
      // mem_write:1-write 2-no write
      // mem_op: 000-1byte,sign  001-2bs  010-4b  100-1bu  101-2bu
      // wd_src: 000-alu_resualt  001-pc+4  010-imm  011-imm+pc 100-mem 101-csr
      // branch: 000-非跳转指令 001-jal, 010-jalr,
      //         100-beq, 101-bnq, 110-blt, 111-bge
      //csr:000-非csr相关  010-ecall  mret-011 101-mret100-mcsrrw  110-mcsrrs,
      10'b?0_???_01101: all_out = 22'b1_011_x_xxxx_0_xxx_010_000_000;  // lui,   u
      10'b?0_???_00101: all_out = 22'b1_011_x_xxxx_0_xxx_011_000_000;  // auipc, u
      10'b?0_???_11011: all_out = 22'b1_100_x_xxxx_0_xxx_001_001_000;  // jal,   j
      10'b?0_000_11001: all_out = 22'b1_000_1_0000_0_xxx_001_010_000;  // jalr,  i
      10'b?0_000_11000: all_out = 22'b0_010_0_1000_0_xxx_xxx_100_000;  // beq,   b
      10'b?0_001_11000: all_out = 22'b0_010_0_1xx0_0_xxx_xxx_101_000;  // bnq,   b
      10'b?0_100_11000: all_out = 22'b0_010_0_1xx0_0_xxx_xxx_110_000;  // blt,   b
      10'b?0_101_11000: all_out = 22'b0_010_0_1xx0_0_xxx_xxx_111_000;  // bge,   b
      10'b?0_110_11000: all_out = 22'b0_010_0_1xx1_0_xxx_xxx_110_000;  // bltu,  b
      10'b?0_111_11000: all_out = 22'b0_010_0_1xx1_0_xxx_xxx_111_000;  // bgeu,  b
      10'b?0_000_00000: all_out = 22'b1_000_1_0000_0_000_100_000_000;  // lb,    i
      10'b?0_001_00000: all_out = 22'b1_000_1_0000_0_001_100_000_000;  // lh,    i
      10'b?0_010_00000: all_out = 22'b1_000_1_0000_0_010_100_000_000;  // lw,    i
      10'b?0_100_00000: all_out = 22'b1_000_1_0000_0_100_100_000_000;  // lbu,   i
      10'b?0_101_00000: all_out = 22'b1_000_1_0000_0_101_100_000_000;  // lhu,   i
      10'b?0_000_01000: all_out = 22'b0_001_1_0000_1_000_xxx_000_000;  // sb,    s
      10'b?0_001_01000: all_out = 22'b0_001_1_0000_1_001_xxx_000_000;  // sh,    s
      10'b?0_010_01000: all_out = 22'b0_001_1_0000_1_010_xxx_000_000;  // sw,    s
      10'b?0_000_00100: all_out = 22'b1_000_1_0000_0_xxx_000_000_000;  // addi,  i
      10'b?0_010_00100: all_out = 22'b1_000_1_1010_0_xxx_000_000_000;  // slti,  i
      10'b?0_011_00100: all_out = 22'b1_000_1_1011_0_xxx_000_000_000;  // sltiu, i
      10'b?0_100_00100: all_out = 22'b1_000_1_x100_0_xxx_000_000_000;  // xori,  i
      10'b?0_110_00100: all_out = 22'b1_000_1_x110_0_xxx_000_000_000;  // ori,   i
      10'b?0_111_00100: all_out = 22'b1_000_1_x111_0_xxx_000_000_000;  // andi,  i
      10'b00_001_00100: all_out = 22'b1_000_1_0001_0_xxx_000_000_000;  // slli,  i
      10'b00_101_00100: all_out = 22'b1_000_1_0101_0_xxx_000_000_000;  // srli,  i
      10'b10_101_00100: all_out = 22'b1_000_1_1101_0_xxx_000_000_000;  // srai,  i
      10'b00_000_01100: all_out = 22'b1_xxx_0_0000_0_xxx_000_000_000;  // add,   r
      10'b10_000_01100: all_out = 22'b1_xxx_0_1000_0_xxx_000_000_000;  // sub,   r
      10'b00_001_01100: all_out = 22'b1_xxx_0_0001_0_xxx_000_000_000;  // sll,   r
      10'b00_010_01100: all_out = 22'b1_xxx_0_1010_0_xxx_000_000_000;  // slt,   r
      10'b00_011_01100: all_out = 22'b1_xxx_0_1011_0_xxx_000_000_000;  // sltu,  r
      10'b00_100_01100: all_out = 22'b1_xxx_0_x100_0_xxx_000_000_000;  // xor,   r
      10'b00_101_01100: all_out = 22'b1_xxx_0_0101_0_xxx_000_000_000;  // srl,   r
      10'b10_101_01100: all_out = 22'b1_xxx_0_1101_0_xxx_000_000_000;  // sra,   r
      10'b00_110_01100: all_out = 22'b1_xxx_0_x110_0_xxx_000_000_000;  // or,    r
      10'b00_111_01100: all_out = 22'b1_xxx_0_x111_0_xxx_000_000_000;  // and,   r
      10'b00_000_11100: all_out = 22'b0_xxx_x_xxxx_0_xxx_xxx_000_010;  // ecall, n
      /*Zicsr*/
      10'b??_001_11100: all_out = 22'b1_000_x_xxxx_0_xxx_101_000_101;  // csrrw,  i
      10'b??_010_11100: all_out = 22'b1_000_x_xxxx_0_xxx_101_000_110;  // csrrs,  i
      /*priveliged*/
      10'b01_000_11100: all_out = 22'b0_xxx_x_xxxx_0_xxx_xxx_000_011;  // mret,  n
      default: all_out = 22'b0;
    endcase
  end

endmodule




// reg_write, imm_src, alu_src, alu_ctr, mem_write, mem_op, wd_src, branch
