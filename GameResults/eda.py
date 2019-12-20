from os import getenv
from dotenv import load_dotenv
import time, json

from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver import ActionChains
from selenium.common import exceptions as selexc
from selenium.webdriver.firefox.options import Options

load_dotenv()

WEBDRIVER_PATH = getenv('WEBDRIVER_PATH')
URL = "https://jutge.org/competitions/EDA:EDA_Q1_2019_20/rounds"
FROM_ROUND = 1
TO_ROUND = 235

options = Options()
options.headless = True
BROWSER = webdriver.Firefox(options=options, executable_path=WEBDRIVER_PATH)

rounds = []

with open(f'./results.json', 'w') as o:
    for i in range(FROM_ROUND, TO_ROUND + 1):
        BROWSER.get(URL)
        time.sleep(5)

        print(f"Getting Round {i}...")
        round_e = BROWSER.find_element_by_xpath(f"//a[text()='Round {i}']")
        round_e.click()
        time.sleep(5)

        compe_e = BROWSER.find_element_by_id("competition_rounds")
        turnsr_e = compe_e.find_elements_by_xpath(".//b")
        turns = [x.get_attribute('innerText')[5:] for x in turnsr_e[0:len(turnsr_e)-1] ] 
        player_lost = turnsr_e[len(turnsr_e)-1].get_attribute('innerText')

        players_e = compe_e.find_elements_by_xpath('.//div[@class="visible-md visible-lg visible-xl"]/div/table/tbody')
        print(f"Round {i} gotten!")

        roundDic = {
            "round": i,
            "num_players": 0,
            "player_loser": player_lost,
            "num_turns": len(turns),
            "turns": [
            ],
        }

        for turn in turns:
            print(f"Getting Turn {turn} results...")
            
            players_i_e = players_e[int(turn)-1].find_elements_by_xpath(".//tr")

            turnDic = {
                "turn": int(turn),
                "num_games": len(players_i_e),
                "games": [
                ],
            }

            if(int(turn) == 1):
                roundDic["num_players"] = len(players_i_e) * 4

            for player_i_e in players_i_e:
                
                gameDic = {
                    "player1_name": "",
                    "player1_pass": False,
                    "player1_points": 0,

                    "player2_name": "",
                    "player2_pass": False,
                    "player2_points": 0,

                    "player3_name": "",
                    "player3_pass": False,
                    "player3_points": 0,

                    "player4_name": "",
                    "player4_pass": True,
                    "player4_points": 0,
                }

                players_i_i_e = player_i_e.find_elements_by_xpath(".//td")[1:]
                player_name = ""
                player_points = 0
                player_pass = False

                for i, player_result in enumerate(players_i_i_e):
                    if(i%2 == 0):
                        player_name = player_result.get_attribute("innerText")
                        player_pass= (player_result.get_attribute("class") != "text-danger")
                    else:
                        player_points = int(player_result.get_attribute("innerText"))
                        if(i == 1):
                            gameDic["player1_name"] = player_name
                            gameDic["player1_pass"] = player_pass
                            gameDic["player1_points"] = player_points
                        elif(i == 3):
                            gameDic["player2_name"] = player_name
                            gameDic["player2_pass"] = player_pass
                            gameDic["player2_points"] = player_points
                        elif(i == 5):
                            gameDic["player3_name"] = player_name
                            gameDic["player3_pass"] = player_pass
                            gameDic["player3_points"] = player_points
                        elif(i == 7):
                            gameDic["player4_name"] = player_name
                            gameDic["player4_pass"] = player_pass
                            gameDic["player4_points"] = player_points

                turnDic["games"].append(gameDic)

            roundDic["turns"].append(turnDic)

        rounds.append(roundDic)

        print(f"Results gotten!")

        print("Getting player that lost...")
        print("Player that lost gotten!")

    o.write(json.dumps(rounds, indent=4))
	
BROWSER.quit()
