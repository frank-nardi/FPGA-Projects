----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/04/2024 04:03:20 PM
-- Design Name: 
-- Module Name: top - Behavioral
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
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top is
    port (
        led: out std_logic_vector(15 downto 0);
        sw: in std_logic_vector(15 downto 0)
    );
end entity top;


architecture Behavioral of top is
    signal led_int: std_logic_vector(15 downto 0);
begin
    led <= led_int;
    led(3 downto 0) <= sw(3 downto 0);
    led(7 downto 4) <= not(sw(3 downto 0));
    
    with sw(8) select
        led(8) <=
            sw(9) when '1',
            sw(10) when '0';
            
    led(9) <= led_int(0) or led_int(1) or led_int(2) or led_int(3) or led_int(4) or led_int(5) or led_int(6) or led_int(7) or not(led_int(0) or led_int(1) or led_int(2) or led_int(3) or led_int(4) or led_int(5) or led_int(6) or led_int(7));
    led(10) <= not(led_int(0) or led_int(1) or led_int(2) or led_int(3) or led_int(4) or led_int(5) or led_int(6) or led_int(7));
    led(15) <= '1';
    led(14) <= '0';
    led(13) <= '1';
    led(12) <= '0';
    led(11) <= '1';
end Behavioral;
