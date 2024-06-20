# AsciiMon
## Pokemon clone on terminal!

![image](https://github.com/WasixXD/AsciiMon/assets/66091116/7cf760db-8bd1-4675-abf4-b43e1ef31577)

**Do you think you can reach the star on the end of the map?**


# Brief 📖
This is a toy replica of the famous Nintendo Game Pokemon, however this version is played on terminal and don't have pretty graphics.
Hardly inspired by: https://github.com/lxgr-linux/pokete

<br>

# Challenges 🐢
- Use ncurses only
- Load Mons automatically
- Battle with trainers
- Battle with wild mons
- NPCs
- Items
<br>

# Goals 🏆
[ x ] First time using C on big project<br>
[ x ] Player can have and capture Mons <br>
[ x ] Player can battle with wild Mons by walking on grass and Trainers in the map<br>
[ x ] Player can use itens and change Mons in the battle<br>
[ x ] Player change Mons before a battle<br>
[ x ] Capture wild mons (**but not trainer mons**) <br>
[ x ] Heal defeated Mons <br>
[ x ] Player can talk with NPCs in the map (**Despite having only one dialog**)  <br>

<br>

# How it works? 💼
So I only use a graphics library called *ncurses* that the main engine behind at all, so after initialize everything every input that the player makes we capture and see if a event has occurred, so if the player is standing in a grass tile ("w") we just calculate a random factor that says if a wild mon wants to battle. In the battle we transfer the control to a new function that handles all the events that can happen (like using a move, or changing the mons order). The battle also can occur when the player enter a trainer sight then again we transfer the control and make the same process. Other things like seeing a item info are just a while loop that display the info while the input are to not close, seems simple but using such a library that have even simplier functions is difficult


<br>

# How to install 🚀

- Be sure to have ncurses installed
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```
- On windows if you use MingW64 or MSYS you are very likely to already have it
___
- Now clone and navigate to the repository folder
```bash
git clone https://github.com/WasixXD/AsciiMon
cd AsciiMon
```
- Be sure to make the `script.sh` executable
  ```bash
  chmod +x ./script.sh
  ```
  
- And just run it with
  ```bash
  ./script.sh
  ```

# ▶ DEMO
## Please play it with the terminal on fullscreen
- Wild battle <br>
![image](https://github.com/WasixXD/AsciiMon/assets/66091116/4c835a07-1e4f-4415-8eb5-08476a65447a)

- Trainer battle <br>
![image](https://github.com/WasixXD/AsciiMon/assets/66091116/f05cf004-aa42-4a28-b590-052674527ed9)

- Displaying your mons <br>
![image](https://github.com/WasixXD/AsciiMon/assets/66091116/ef40fd20-680e-456b-9b76-3e6ee5ae61e6)

- Seeing info about attacks <br>
![image](https://github.com/WasixXD/AsciiMon/assets/66091116/cc86f326-fe0a-4af1-883e-0a60da50731e)

- Seeing info about itens <br>
![image](https://github.com/WasixXD/AsciiMon/assets/66091116/7ec4add7-bfde-44bd-b967-0b944c6f8050)


PS: Pokémon, I Choose You!

