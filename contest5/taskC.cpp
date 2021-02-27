#include <iostream>
#include <vector>
 
bool in(int x, int y, int W, int H) {
    return x < W && x >= 0 && y < H && y >= 0;
}
 
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int W, H, N;
    std::cin >> W >> H >> N;
    if (static_cast<long long>(W) * H  > static_cast<long long>(N) * 5) {
        std::cout << "No";
        return 0;
    }
    std::vector<bool> canvas(W*H, false);
    int x, y;
    for (int i = 0; i < N; ++i) {
        std::cin >> x >> y;
        --x;
        --y;
        if (in(x, y, W, H)) canvas[x + y * W] = true;
        if (in(x+1, y, W, H)) canvas[x+1 + y * W] = true;
        if (in(x, y+1, W, H)) canvas[x + (y+1) * W] = true;
        if (in(x-1, y, W, H)) canvas[x-1 + y * W] = true;
        if (in(x, y-1, W, H)) canvas[x + (y-1) * W] = true;
 
    }
    for (auto && canva : canvas) {
        if (!canva) {
            std::cout << "No";
            return 0;
        }
    }
    std::cout << "Yes";
    return 0;
}
