extern unsigned char rom_sky_destroyer_nes[];
extern unsigned char rom_starforce_nes[];
extern unsigned char rom_bomberman_nes[];
extern unsigned char rom_tetris_nes[];
extern unsigned char rom_pacman_nes[];
extern unsigned char rom_mario_nes[];
extern unsigned char rom_stargate_nes[];
extern unsigned char rom_pinball_nes[];
extern unsigned char rom_karateka_nes[];
extern unsigned char rom_balloon_nes[];
extern unsigned char rom_kungfu_nes[];
extern unsigned char rom_exerion_nes[];
extern unsigned char rom_f1_nes[];
extern unsigned char rom_lunar_pool_nes[];
extern unsigned char rom_spartanx_nes[];

struct rom {
  const char *name;
  void *body;
};

struct rom roms[] = {
  { .name = "sky_destroyer", .body = rom_sky_destroyer_nes, },
  { .name = "starforce", .body = rom_starforce_nes, },
  { .name = "bomberman", .body = rom_bomberman_nes, },
  { .name = "tetris", .body = rom_tetris_nes, },
  { .name = "pacman", .body = rom_pacman_nes, },
  { .name = "mario", .body = rom_mario_nes, },
  { .name = "stargate", .body = rom_stargate_nes, },
  { .name = "pinball", .body = rom_pinball_nes, },
  { .name = "karateka", .body = rom_karateka_nes, },
  { .name = "balloon", .body = rom_balloon_nes, },
  { .name = "kungfu", .body = rom_kungfu_nes, },
  { .name = "exerion", .body = rom_exerion_nes, },
  { .name = "f1", .body = rom_f1_nes, },
  { .name = "lunar_pool", .body = rom_lunar_pool_nes, },
  { .name = "spartanx", .body = rom_spartanx_nes, },
};
int nroms = 15;
