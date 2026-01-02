library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity pulser is
  Port (
  a:in std_logic;
  clk:in std_logic;
  a_pulse:out std_logic);
end pulser;

architecture Behavioral of pulser is
    signal qPulser1:std_logic;
    signal qPulser2:std_logic;
begin
    process (clk) begin
        if rising_edge(clk) then
            qPulser1 <= a;
        end if;
    end process;
   
    process (clk) begin
        if rising_edge(clk) then
            qPulser2 <= qPulser1;
        end if;
    end process;
   
    a_pulse<=qPulser1 and not(qPulser2);
end Behavioral;