----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 25.09.2024 15:16:47
-- Design Name:
-- Module Name: Debouncer - Behavioral
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description:
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Debouncer is
     Port (
     a_debounce: out std_logic;
     clk: in std_logic;
     button: in std_logic);
end Debouncer;

architecture Behavioral of Debouncer is
    signal count:std_logic_vector(18 downto 0);
    signal clr:std_logic;
    signal en:std_logic;
   
begin
    process (clk) begin
        if rising_edge(clk) then
                count<=std_logic_vector(unsigned(count)+1);
        end if;
    end process;

               
    en<='1' when count = "1111111111111111111";
    process (clk) begin
        if rising_edge(clk) then
            if en='1' then
                a_debounce<=button;
             end if;
        end if;
    end process;
end Behavioral;