# wordle_solver
A way to solve your daily wordle in C.

It is a naive way to solve the puzzle. It does not do grouping or other things that would help you decide the next guess.

# Compile
This has only been teste on a mac using clang.

```
 clang *.c -o wordle_solver
```
# Run
From the console: 
```
./wordle_solver
```

## How to input data
After you guess in the wordle app. You will have the guess color coded. This is needed to use the `wordle_solver`. 

In the solver, you will be asked for the word you guess. Enter the 5 letter word.
You will then be asked to enter the hint string which is how you provide the color feedback. In order enter the corresponding color; `y` for yellow and `g` for green.
Anything other will be a *. This will trimming the dictionary list of remaining words. 

# Areas to improve
* Group remaining words to help aid future guesses.
* Add/remove more words based on the office wordle list.
