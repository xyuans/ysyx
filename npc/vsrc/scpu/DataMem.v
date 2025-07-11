import "DPI-C" function int pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input unsigned waddr, input unsigned wdata, input int len);

module DataMem (
  input we,
  input clk,
  input [2:0] ctr,
  input [31:0] addr,
  input [31:0] wd,
  output reg [31:0] rd
);
  // mem_op: 000-1byte,sign  001-2bs  010-4b  100-1bu  101-2bu
  reg [31:0] read_data;
  always @(*) begin
    read_data = pmem_read(addr);
  end
  always @(posedge clk) begin
    case (ctr)
      3'b000: begin 
        rd <= {{24{read_data[7] }}, read_data[7:0] }; 
        if (we) begin
          pmem_write(addr, wd, 1);
        end
      end
      3'b001: begin 
        rd <= {{16{read_data[15]}}, read_data[15:0]};
        if (we) begin
          pmem_write(addr, wd, 2);
        end
      end
      3'b010: begin
        rd <= read_data;
        if (we) begin
          pmem_write(addr, wd, 4);
      end
      3'b100: begin
        rd <= {24'b0, read_data[7]};
        if (we) begin
          pmem_write(addr, wd, 1);
        end
      end
      3'b101: begin
        rd <= {16'b0, read_data[15:0]};
        if (we) begin
          pmem_write(addr, wd, 2);
        end
      end
    endcase
  
  end
endmodule
