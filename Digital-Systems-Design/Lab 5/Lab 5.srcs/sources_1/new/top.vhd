----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/09/2024 03:22:34 PM
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top is
    port ( 
        an: out std_logic_vector(3 downto 0);
        seg: out std_logic_vector(6 downto 0);
        sw: in std_logic_vector(15 downto 0);
        clk: in std_logic;
        btnC: in std_logic
    );
end top;

architecture Behavioral of top is
    component hex_to_seven is Port (
        hex_in: in std_logic_vector(3 downto 0);
        segment_out: out std_logic_vector (6 downto 0)
    );
    end component;
    
    component seven_segment_display_driver is Port (
        clk: in std_logic;
        an: out std_logic_vector(3 downto 0);
        seg0_data: in std_logic_vector(6 downto 0);
        seg1_data: in std_logic_vector(6 downto 0);
        seg2_data: in std_logic_vector(6 downto 0);
        seg3_data: in std_logic_vector(6 downto 0);
        seg: out std_logic_vector(6 downto 0)        
    );
    end component;
    
    component Pulser is Port (
        a:in std_logic;
        clk:in std_logic;
        a_pulse:out std_logic);
    end component;
    
    component Debouncer is Port (
        a_debounce: out std_logic;
        clk: in std_logic;
        button: in std_logic);
    end component;
signal pulse_out:std_logic;
signal Q:std_logic_vector(1 downto 0);
signal debounce_out:std_logic;
signal inc:std_logic_vector(15 downto 0);
signal seg0:std_logic_vector(6 downto 0);
signal seg1:std_logic_vector(6 downto 0);
signal seg2:std_logic_vector(6 downto 0);
signal seg3:std_logic_vector(6 downto 0);

begin
U1: Debouncer port map(clk=>clk,button=>btnC,a_debounce=>debounce_out);
U2: Pulser port map(a=>debounce_out,clk=>clk,a_pulse=>pulse_out);

process(clk) begin
    if rising_edge(clk) then
        if pulse_out = '1' then
            inc<= std_logic_vector(unsigned(inc)-1);
        end if;
    end if;
end process;
U3: hex_to_seven port map(hex_in=>inc(3 downto 0),segment_out=>seg0);
U4: hex_to_seven port map(hex_in=>inc(7 downto 4),segment_out=>seg1);
U5: hex_to_seven port map(hex_in=>inc(11 downto 8),segment_out=>seg2);
U6: hex_to_seven port map(hex_in=>inc(15 downto 12),segment_out=>seg3);
U7: seven_segment_display_driver port map(clk=>clk,an=>an,seg0_data=>seg0,seg1_data=>seg1,seg2_data=>seg2,seg3_data=>seg3,seg=>seg);

end Behavioral;
