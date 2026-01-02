----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/09/2024 04:35:18 PM
-- Design Name: 
-- Module Name: seven_segment_display_driver - Behavioral
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

entity seven_segment_display_driver is
    Port (
        clk: in std_logic;
        an: out std_logic_vector(3 downto 0);
        seg0_data: in std_logic_vector(6 downto 0);
        seg1_data: in std_logic_vector(6 downto 0);
        seg2_data: in std_logic_vector(6 downto 0);
        seg3_data: in std_logic_vector(6 downto 0);
        seg: out std_logic_vector(6 downto 0)
    );
end seven_segment_display_driver;

architecture Behavioral of seven_segment_display_driver is
signal Q: std_logic_vector(1 downto 0);
signal count: std_logic_vector(18 downto 0);
signal en: std_logic;
signal clr: std_logic;

begin
    with Q select an <=
        "1110" when "00",
        "1101" when "01",
        "1011" when "10",
        "0111" when "11";
    
    with Q select seg <=
        seg0_data when "00",
        seg1_data when "01",
        seg2_data when "10",
        seg3_data when "11";
    
    process (clk) begin
        if rising_edge(clk) then
            count <= std_logic_vector(unsigned(count)-1);
        end if;
    end process;
    
    en <= '1' when count = "1111111111111111111" else '0';
    process (clk) begin
        if rising_edge(clk) then
            if en='1' then
                Q <= std_logic_vector(unsigned(Q)+1) ;
             end if;
        end if;
    end process;
end Behavioral;
