#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEAMS 20

// Define a struct for a team
typedef struct {
    char* name;
    int matches_played;
    int wins;
    int ties;
    int defeats;
    int goals_scored;
    int goals_granted;
    int goal_difference;
    int points;
} Team;

// Define a struct for a game
typedef struct {
    char* home_team;
    int home_goals;
    char* away_team;
    int away_goals;
} Game;

// Define a function to find a team in the array of teams
int find_team(Team* teams, int num_teams, char* name) {
    for (int i = 0; i < num_teams; i++) {
        if (strcmp(teams[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Define a function to add a new team to the array of teams
int add_team(Team* teams, int* num_teams, char* name) {
    if (*num_teams >= MAX_TEAMS) {
        return -1;
    }
    teams[*num_teams].name = (char*)malloc(strlen(name) + 1);
    strcpy(teams[*num_teams].name, name);
    teams[*num_teams].matches_played = 0;
    teams[*num_teams].wins = 0;
    teams[*num_teams].ties = 0;
    teams[*num_teams].defeats = 0;
    teams[*num_teams].goals_scored = 0;
    teams[*num_teams].goals_granted = 0;
    teams[*num_teams].goal_difference = 0;
    teams[*num_teams].points = 0;
    (*num_teams)++;
    return (*num_teams) - 1;
}

// Define a function to update the league table based on a game result
void update_table(Team* teams, int* num_teams, char* home_team, int home_goals, char* away_team, int away_goals) {
    int home_index = find_team(teams, *num_teams, home_team);
    if (home_index == -1) {
        home_index = add_team(teams, num_teams, home_team);
    }
    int away_index = find_team(teams, *num_teams, away_team);
    if (away_index == -1) {
        away_index = add_team(teams, num_teams, away_team);
    }
    teams[home_index].matches_played++;
    teams[away_index].matches_played++;
    teams[home_index].goals_scored += home_goals;
    teams[away_index].goals_scored += away_goals;
    teams[home_index].goals_granted += away_goals;
    teams[away_index].goals_granted += home_goals;
    teams[home_index].goal_difference = teams[home_index].goals_scored - teams[home_index].goals_granted;
    teams[away_index].goal_difference = teams[away_index].goals_scored - teams[away_index].goals_granted;
    if (home_goals > away_goals) {
        teams[home_index].wins++;
        teams[away_index].defeats++;
        teams[home_index].points += 3;
    }
    else if (home_goals == away_goals) {
        teams[home_index].ties++;
        teams[away_index].ties++;
        teams[home_index].points += 1;
        teams[away_index].points += 1;
    }
    else {
        teams[home_index].defeats++;
        teams[away_index].wins++;
        teams[away_index].points += 3;
    }
}

// Define a function to swap two teams in the array of teams
void swap_teams(Team* teams, int i, int j) {
    Team temp = teams[i];
    teams[i] = teams[j];
    teams[j] = temp;
}

// Define the quicksort algorithm to sort the league table
void quicksort(Team* teams, int num_teams, int low, int high) {
    int i = low;
    int j = high;
    int pivot = teams[(low + high) / 2].points;
    int pivot_diff = teams[(low + high) / 2].goal_difference;
    while (i <= j) {
        while (teams[i].points > pivot || (teams[i].points == pivot && teams[i].goal_difference > pivot_diff)) {
            i++;
        }
        while (teams[j].points < pivot || (teams[j].points == pivot && teams[j].goal_difference < pivot_diff)) {
            j--;
        }
        if (i <= j) {
            swap_teams(teams, i, j);
            i++;
            j--;
        }
    }
    if (low < j) {
        quicksort(teams, num_teams, low, j);
    }
    if (i < high) {
        quicksort(teams, num_teams, i, high);
    }
}

int main() {
    // Declare the array of teams
    Team teams[MAX_TEAMS];
    int num_teams = 0;

    // Open the CSV file and read the data
    FILE* fp = fopen("database_game.csv", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    char line[1024];
    while (fgets(line, 1024, fp)) {
        char* date_str = strtok(line, ",");
        char* home_team = strtok(NULL, ",");
        char* home_goals_str = strtok(NULL, ",");
        char* away_team = strtok(NULL, ",");
        char* away_goals_str = strtok(NULL, ",\n");
        if (home_team != NULL && home_goals_str != NULL && away_team != NULL && away_goals_str != NULL) {
            int home_goals = atoi(home_goals_str);
            int away_goals = atoi(away_goals_str);
            update_table(teams, &num_teams, home_team, home_goals, away_team, away_goals);
        }
    }
    fclose(fp);

    // Sort the league table by points and goal difference using quicksort
    quicksort(teams, num_teams, 0, num_teams - 1);

    // Print out the league table
    printf("%-10s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n", "Team Name", "GP", "W", "D", "L", "GS", "GC", "GD", "Pts");
    for (int i = 0; i < num_teams; i++) {
        printf("%-10s %-5d %-5d %-5d %-5d %-5d %-5d %-5d %-5d \n",
            teams[i].name,
            teams[i].matches_played,
            teams[i].wins,
            teams[i].ties,
            teams[i].defeats,
            teams[i].goals_scored,
            teams[i].goals_granted,
            teams[i].goal_difference,
            teams[i].points);
    }

    // Free the memory allocated for the team names
    for (int i = 0; i < num_teams; i++) {
        free(teams[i].name);
    }

    return 0;
}


