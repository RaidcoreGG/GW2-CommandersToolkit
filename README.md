[![](https://discordapp.com/api/guilds/410828272679518241/widget.png?style=banner2)](https://discord.gg/Mvk7W7gjE4)
[![](https://raidcore.gg/Resources/Images/Patreon.png)](https://www.patreon.com/bePatron?u=46163080)

![](https://img.shields.io/github/license/RaidcoreGG/GW2-CommandersToolkit?style=for-the-badge&labelColor=%23131519&color=%230F79AA)
![](https://img.shields.io/github/v/release/RaidcoreGG/GW2-CommandersToolkit?style=for-the-badge&labelColor=%23131519&color=%230F79AA)
![](https://img.shields.io/github/downloads/RaidcoreGG/GW2-CommandersToolkit/total?style=for-the-badge&labelColor=%23131519&color=%230F79AA)

# GW2-CommandersToolkit
A simple arcdps module that tracks squad members, so that you can check if someone left the party what they were responsible for.

![alt text](https://i.imgur.com/dqEmEae.png)

## Usage
When someone joins your squad simply tick off what they're responsible for and set their subgroup.
This way you can see whether Subgroup X has the main boons, as well as Heal and Vulnerability covered.
You can also write down a quick note, to remind yourself for example who will later do Handkite or Pylon.

If everything in a subgroup is covered, the text will be in green. If you have more than 5 members in a sub, you will be shown a warning.

When a user goes out of scope (leaves instance or squad), they will be shown in red and you can click to remove them. Once they go into scope again, they will no longer be marked.

## Templates
![alt text](https://i.imgur.com/HFnTWg5.png)

For ease of usability you can also apply the coverage of someone based on their build. If a Heal Alacrity Mechanist for example joins, you can select that from the context menu and it will be filled out from there.

## Limitations
Subgroups have to be manually set for each user, as they are only exposed by arc when entering combat or when changing the instance.
