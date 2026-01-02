----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/18/2024 03:12:19 PM
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
        sw: in std_logic_vector(15 downto 0);
        clk: in std_logic;
        btnC: in std_logic
    );
end entity top;

architecture Behavioral of top is
    signal qPulser1: std_logic;
    signal qPulser2: std_logic;
    signal s0: std_logic;
    signal s1: std_logic;
    signal enable: std_logic;
    signal reg1: std_logic;
    signal reg2: std_logic;
    signal reg3: std_logic;
begin
    -- Pulser begin
    process (clk) begin
        if rising_edge(clk) then
            qPulser1 <= btnC;
        end if;
    end process;
    
    process (clk) begin
        if rising_edge(clk) then
            qPulser2 <= qPulser1;
        end if;
    end process;
    
    enable <= qPulser1 and not(qPulser2);
    -- Pulser end
    
    process (clk) begin
        if rising_edge(clk) then
            if enable = '1' then
                reg1 <= sw(0);
            end if;
        end if;
    end process;
    
    process (clk) begin
        if rising_edge(clk) then
            if enable = '1' then
                reg2 <= reg1;
            end if;
        end if;
    end process;
    
    process (clk) begin
        if rising_edge(clk) then
            if enable = '1' then
                reg3 <= reg2;
            end if;
        end if;
    end process;
               
    led(15) <= not(reg1) and reg2 and reg3;

    process (clk) begin
        if rising_edge(clk) then
            if enable = '1' then
                s1 <= s0;
            end if;
        end if;
    end process;
    
    process (clk) begin
        if rising_edge(clk) then
            if enable = '1' then
                s0 <= not(s1);
            end if;
        end if;
    end process;
    
    led(1 downto 0) <= s1 & s0;
                
end Behavioral;
