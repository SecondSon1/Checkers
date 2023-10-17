# Checkers
Russian Checkers game with inbuilt bots

Language: C++

GUI Framework: SFML

Warning: Calculating bot by default has depth of tree of evaluation
of 9. Works ok on my M1 Pro machine, in general works great, only
once encountered move over which it had to compute for over 5 seconds.
However, on weaker computers you might want to lower that value
(src/player/bot/calculating_bot/calculating_bot.cpp)