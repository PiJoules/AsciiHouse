#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <time.h>

#define BRICK '*'

using namespace std;

void printBlueprint(vector<string>& blueprint){
    for (int i = 0; i < blueprint.size(); i++){
    	cout << blueprint[i] << endl;
    }
}

void replaceArea(vector<string>& blueprint, int x0, int y0){
	// Corners
	blueprint[y0*2].replace(x0*4, 1, "+");
	blueprint[y0*2].replace(x0*4+4, 1, "+");
	blueprint[y0*2+2].replace(x0*4, 1, "+");
	blueprint[y0*2+2].replace(x0*4+4, 1, "+");

	// Sides
	blueprint[y0*2+1].replace(x0*4, 1, "|");
	blueprint[y0*2+1].replace(x0*4+4, 1, "|");

	// Tops and bottoms
	blueprint[y0*2].replace(x0*4+1, 3, "---");
	blueprint[y0*2+2].replace(x0*4+1, 3, "---");
}

void makeRoof(vector<string>& blueprint, int w, int x0, int y0, int* start){
	// x4 for blueprint height; +1 for the extra brick;
	// -3 for the side bricks and A on top; /2 for the number of roof panels
	int roofLen = (w*4+1-3)/2;
	int roofHeight = 2*w;
	int height = blueprint.size();

	// Adjust height of blueprint
	while (blueprint.size() < height-y0*2+roofHeight){
		blueprint.insert( blueprint.begin(), string(blueprint[0].length(), ' ') );
		*start += 1;
	}

	// Left panels
	for (int dx = 0; dx < roofLen; dx++){
		blueprint[*start+(y0*2-1-dx)].replace(x0*4+1+dx, 1, "/");
	}

	// Right panels
	for (int dx = 0; dx < roofLen; dx++){
		blueprint[*start+(y0*2-1-dx)].replace(x0*4+w*4-1-dx, 1, "\\");
	}

	// Ayy
	blueprint[*start+(y0*2-1-roofLen)].replace(x0*4+1+roofLen, 1, "A");
}

/**
 * Remove |, +, -
 */
void cleanup(vector<string>& blueprint){
	for (int i = 0; i < blueprint.size(); i++){
		for (int j = 0; j < blueprint[i].length(); j++){
			char c = blueprint[i].at(j);
			if (c == '|' && j > 0 && j < blueprint[i].length()-1){
				if (blueprint[i-1].at(j-1) == '-' && blueprint[i-1].at(j+1) == '-' && blueprint[i+1].at(j-1) == '-' && blueprint[i+1].at(j+1) == '-'){
					blueprint[i].replace(j,1," ");
				}
			}
			else if (c == '+' && i > 0 && i < blueprint.size()-1 && j > 0 && j < blueprint[i].length()-1){
				if (blueprint[i-2].at(j-1) == '-' && blueprint[i-2].at(j+1) == '-' && blueprint[i+2].at(j-1) == '-' && blueprint[i+2].at(j+1) == '-'){
					blueprint[i].replace(j,1," ");
				}
			}
		}
	}

	int deletions = 0;
	while (1){
		for (int i = 0; i < blueprint.size(); i++){
			for (int j = 0; j < blueprint[i].length(); j++){
				char c = blueprint[i].at(j);
				if (c == '|' && j > 0 && j < blueprint[i].length()-1){
					if (blueprint[i-1].at(j) == ' ' || blueprint[i+1].at(j) == ' '){
						blueprint[i].replace(j,1," ");
						deletions++;
					}
				}
				else if (c == '-' && i > 0 && i < blueprint.size()-1){
					if (blueprint[i].at(j-1) == ' ' || blueprint[i].at(j+1) == ' '){
						blueprint[i].replace(j,1," ");
						deletions++;
					}
					else if (blueprint[i-2].at(j) == '-' && blueprint[i+2].at(j) == ' '){
						blueprint[i].replace(j,1," ");
						deletions++;
					}
				}
			}
		}
		if (deletions > 0){
			deletions = 0;
		}
		else {
			break;
		}
	}
}

int main(int argc, char* argv[]){
	string buffer;
	getline(cin, buffer);
	int lineCount = atoi(buffer.c_str());
	string lines[lineCount];
	for (int i = 0; i < lineCount; i++){
        string line;
        getline(cin, line);
        lines[i] = line;
    }

    int width = 4*lines[lineCount-1].size()+1;
    int height = 2*lineCount+1;
    vector<string> blueprint;
    for (int i = 0; i < height; i++){
    	blueprint.push_back(string(width, ' '));
    }

    // Replace blocks
    for (int y = 0; y < lineCount; y++){
    	for (int x = 0; x < lines[y].size(); x++){
    		char c = lines[y].at(x);
    		if (c == BRICK){
    			// Replace area in blueprint
    			replaceArea(blueprint, x, y);
    		}
    	}
    }
    cleanup(blueprint);

    // Start with roofs on the top line
    int start = 0;
    bool startOfRoof = false;
    int w = 0;
	for (int j = 0; j < lines[0].length(); j++){
		char c = lines[0].at(j);
		if (c == BRICK && !startOfRoof){
			w++;
			startOfRoof = true;
		}
		else if (startOfRoof && c == BRICK){
			w++;
		}
		else if ( startOfRoof && c == ' '){
			startOfRoof = false;
			makeRoof(blueprint, w, j-w, 0, &start);
			w = 0;
		}
	}
	if (w > 0){
		makeRoof(blueprint, w, lines[0].length()-w, 0, &start);
		w = 0;
		startOfRoof = false;
	}

    // Now finish the rest
    for (int i = 1; i < lineCount; i++){
    	for (int j = 0; j < lines[i].length(); j++){
    		char c = lines[i].at(j);
    		if (c == BRICK && !startOfRoof && j >= lines[i-1].length()){
    			w++;
    			startOfRoof = true;
    		}
      		else if (c == BRICK && !startOfRoof && lines[i-1].at(j) == ' '){
    			w++;
    			startOfRoof = true;
    		}
    		else if (startOfRoof && c == BRICK && j >= lines[i-1].length()){
    			w++;
    		}
    		else if (startOfRoof && c == BRICK && lines[i-1].at(j) == ' '){
    			w++;
    		}
    		else if ( startOfRoof && (c == ' ' || lines[i-1].at(j) == BRICK) ){
    			startOfRoof = false;
    			makeRoof(blueprint, w, j-w, i, &start);
    			w = 0;
    		}
    	}
    	if (w > 0){
    		makeRoof(blueprint, w, lines[i].length()-1, i, &start);
    		w = 0;
    		startOfRoof = false;
    	}
    }

    // Add door
    srand ( time(NULL) ); // Seed rand
    int doorIndex = (rand() % (blueprint[blueprint.size()-1].length()-4)) + 1;
    blueprint[blueprint.size()-2].replace(doorIndex, 3, "| |");

    // Add windows
    for (int i = 0; i < blueprint.size(); i++){
    	bool inHouse = false;
    	for (int j = 0; j < blueprint[i].size(); j++){
    		char c = blueprint[i].at(j);
    		if (!inHouse && c == '|'){
    			inHouse = true;
    		}
    		else if (inHouse && c == ' '){
    			srand ( time(NULL) + i*31 + j );
    			if (rand() % 2 == 0 && blueprint[i].at(j-1) != '|' && blueprint[i].at(j+1) != '|'){
    				blueprint[i].replace(j,1,"o");
    			}
    		}
    		else if (inHouse && c == '|'){
    			inHouse = false;
    		}
    	}
    }

    printBlueprint(blueprint);

    return 0;
}