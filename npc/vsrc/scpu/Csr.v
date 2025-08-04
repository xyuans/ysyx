// 读写都在上升沿发生
// ctr:000-非csr相关  010-ecall  mret-011 101-mret100-mcsrrw  110-mcsrrs
module Csr (
    input clk,
    input [31:0] pc,
    input [2:0] ctr,
    input [11:0] addr,
    input [31:0] wd,
    output [31:0] rd,
    output reg [31:0] pc_out
);
  reg [31:0] mstatus, mtvec, mepc, mcause;
  reg [31:0] csr_rd, csr_wd;

  always @(*) begin
    case (ctr[0])  // ctr[0]-0 ecall ctr[0]-1 mret
      1'b0: pc_out = mtvec;
      1'b1: pc_out = mepc;
      default: pc_out = 32'bx;
    endcase
  end

  // csr读
  always @(*) begin
    case (addr)
      12'h300: csr_rd = mstatus;
      12'h305: csr_rd = mtvec;
      12'h341: csr_rd = mepc;
      12'h342: csr_rd = mcause;
      default: csr_rd = 32'b0;
    endcase
  end
  assign rd = csr_rd;
  assign csr_wd = wd | (csr_rd & {32{ctr[1]}});  // ctr[1]=0时，wd | 0。ctr[1]=1, wd | csr_rd。

  // csr写模块
  always @(posedge clk) begin
    if (ctr[2]) begin
      case (addr)
        12'h300: mstatus <= csr_wd;
        12'h305: mtvec <= csr_wd;
        12'h341: mepc <= csr_wd;
        12'h342: mcause <= csr_wd;
        default: csr_wd <= csr_wd;
      endcase
    end

    if (~ctr[2] & ctr[1] & ~ctr[0]) begin
      mepc   <= pc;
      mcause <= 32'd11;
    end
  end
endmodule
