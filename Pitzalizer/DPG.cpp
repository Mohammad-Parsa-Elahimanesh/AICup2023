#include <bits/stdc++.h>

#define OUTPUT
//#define GRAPGICS
//#define ANIMATION

#ifdef GRAPHICS
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#endif

using namespace std;

typedef long long ll;
typedef long double ld;
typedef pair<int,int> pii;

#define vc vector
#define pb push_back
#define all(c) (c).begin(), (c).end()

#define mp make_pair
#define fr first
#define sc second

#define endl "\n"

const int W = 500 , MG = 20 , ITER = 1000;

#ifdef GRAPHICS
struct Color{
    int r , g , b , a;
};
const Color NodeColor = Color{0 , 0 , 0 , 255};
const Color EdgeColor = Color{110 , 124 , 145 , 255};

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        // Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}
#endif

int main(){
    #ifdef GRAPHICS
    SDL_Window *window;
    SDL_Renderer* rendrr;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n" , SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("DPG" , SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED , W + 100 , W + 100 , 0);
    if(!window){
        printf("Error creating window: %s\n" , SDL_GetError());
        SDL_Quit();
        return 0;
    }
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    rendrr = SDL_CreateRenderer(window , -1 , render_flags);
    if(!rendrr){
        printf("Error creating renderer: %s\n" , SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    #endif

    int n , m;
    cin >> n >> m;
    int adj[n][n] = {};
    for(int i = 0 ; i < m ; i++){
        int a , b;
        cin >> a >> b;
        adj[a][b] = adj[b][a] = 1;
    }

    ld x[n] , y[n];
    for(int i = 0 ; i < n ; i++) x[i] = 0.5 + sin((ld)i / n * 2 * 3.1415926) , y[i] = 0.5 + cos((ld)i / n * 2 * 3.1415926);
    
    for(int _ = 0 ; _ < ITER ; _++){
        #ifdef ANIMATION
        usleep(1e5);
        #endif

        ld tfx[n] = {} , tfy[n] = {};
        for(int i = 0 ; i < n ; i++){
            for(int j = 0 ; j < n ; j++){
                ld r = sqrt((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]));
                if(r < 1) continue;
                ld f = adj[i][j] * -sqrt(log(r)) / 10 + 10 / (r);
                ld fx = (x[i] - x[j]) / r * f , fy = (y[i] - y[j]) / r * f;
                tfx[i] += fx , tfy[i] += fy;
            }
            {
                //ld xc = 0.5 + sin((ld)i / n * 2 * 3.1415926) , yc = 0.5 + cos((ld)i / n * 2 * 3.1415926);
                ld xc = (ld)W / 2 , yc = (ld)W / 2;
                ld r = sqrt((x[i] - xc) * (x[i] - xc) + (y[i] - yc) * (y[i] - yc));
                if(r < 1) continue;
                ld f = r * sqrt(r) / 1000;
                ld fx = (x[i] - xc) / r * f , fy = (y[i] - yc) / r * f;
                tfx[i] += fx , tfy[i] += fy;
            }
        }
        for(int i = 0 ; i < n ; i++){
            x[i] += tfx[i] , y[i] += tfy[i];
        }

        ld mnx = 1e9 , mxx = 0, mny = 1e9, mxy = 0;
        for(int i = 0 ; i < n ; i++) 
            mnx = min(mnx , x[i]) , mny = min(mny , y[i]) , mxx = max(mxx , x[i]) , mxy = max(mxy , y[i]);
        for(int i = 0 ; i < n ; i++){
            x[i] = (x[i] - mnx) / (mxx - mnx) * W;
            y[i] = (y[i] - mny) / (mxy - mny) * W;
        }

        #ifdef GRAPHICS
        if(_ == ITER-1){
            SDL_SetRenderDrawColor(rendrr , 255 , 255 , 255 , 255);
            SDL_RenderClear(rendrr);
            for(int i = 0 ; i < n ; i++){
                SDL_SetRenderDrawColor(rendrr , NodeColor.r , NodeColor.g , NodeColor.b , NodeColor.a);
                DrawCircle(rendrr , x[i] , y[i] , 8);
                for(int j = i+1 ; j < n ; j++){
                    if(adj[i][j]) SDL_RenderDrawLine(rendrr , x[i] , y[i] , x[j] , y[j]);
                }
            }
            SDL_RenderPresent(rendrr);
        }
        #endif

    }
    #ifdef OUTPUT
    cout << n << " " << m << '\n';
    for(int node, score, i = 0; i < 6; i++) {
        cin >> node >> score;
        cout << node << " " << score << "\n";
    }
    for(int i = 0 ; i < n ; i++){
        cout << x[i]/W << " " << y[i]/W << '\n';
    }
    for(int i = 0 ; i < n ; i++){
        for(int j = i+1 ; j < n ; j++){
            if(adj[i][j]) cout << i << " " << j << '\n';
        }
    }
    cout << "\n\n\n";
    #endif

    #ifdef GRAPHICS
    usleep(1e8);
    #endif
}