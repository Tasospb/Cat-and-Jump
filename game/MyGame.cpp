#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) :
	m_player(400, 540, 0, 0, 0),
	background(400,300, "background.png",CColor::White(),0),
	startScreen(400, 300, "startScreen.bmp", CColor::White(), 0)
	// to initialise more sprites here use a comma-separated list
	
{
	// TODO: add initialisation here
	int level = 0;
	
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)


void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();


	if (m_player.IsDead())
	{
		m_player.UnDie();
		m_player.UnDelete();
		NewGame();
	}
	else if (m_player.IsDying())
	{
		m_player.Update(t);
		for (CSprite* pSprite : m_sprites)
			pSprite->Update(t);
		deathsound.Play("deathsound.wav");
	}
	else
	{

		//gravity for the player
		if (m_state == AIRBORNE )
			m_player.Accelerate(0, -50);

		// Run and Stand
		if (m_state == STANDING || m_state == RUNNING)
		{
			m_player.SetVelocity(0, 0);
			{
				if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
				{
					m_player.Accelerate(-300, 0);
					if (m_state != RUNNING || m_side != LEFT)
						m_player.SetAnimation("run_left");
					m_state = RUNNING;
					m_side = LEFT;
				}
				else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
				{
					m_player.Accelerate(300, 0);
					if (m_state != RUNNING || m_side != RIGHT)
						m_player.SetAnimation("run_right");
					m_state = RUNNING;
					m_side = RIGHT;
				}
				else
				{
					if (m_state == RUNNING)
						m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
					m_state = STANDING;
				}
			}
		}

		if (m_state == FLYING)
		{
			if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
			{
				m_player.Accelerate(-300, 0);
				if (m_state != RUNNING || m_side != LEFT)
					m_player.SetAnimation("run_left");
				m_state = RUNNING;
				m_side = LEFT;
			}
			else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
			{
				m_player.Accelerate(300, 0);
				if (m_state != RUNNING || m_side != RIGHT)
					m_player.SetAnimation("run_right");
				m_state = RUNNING;
				m_side = RIGHT;
			}
			else
			{
				if (m_state == RUNNING)
					m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
				m_state = STANDING;
			}

		}



		// Pre-Update Position
		CVector v0 = m_player.GetPos();

		// Updates
		for (CSprite* pSprite : m_sprites)
			pSprite->Update(t);
		m_player.Update(t);

		int h = m_player.GetHeight() / 2 - 1;
		bool bTouchingPlatform = false;
		// top collision detection
		for (CSprite* pSprite : m_sprites)
			if (m_player.HitTest(pSprite, 0))
			{
				//platforms and walls
				if ((string)pSprite->GetProperty("tag") == "platform")
				{
					if (v0.m_y >= pSprite->GetTop() + h)
					{
						m_player.SetVelocity(0, 0);
						m_player.SetY(pSprite->GetTop() + h);
						bTouchingPlatform = true;
					}
				}
			}

		//bottom collision detection
		for (CSprite* pSprite : m_sprites)
			if (m_player.HitTest(pSprite, 0))
			{
				//platforms and walls
				if ((string)pSprite->GetProperty("tag") == "platform")
				{
					if (v0.m_y <= pSprite->GetBottom() + h)
					{
						m_player.SetVelocity(0, 0);
					}
				}
			}

		int w = m_player.GetWidth() / 2;
		//left collision detection
		for (CSprite* pSprite : m_sprites)
			if (m_player.HitTest(pSprite, 0))
			{
				//platforms and walls
				if ((string)pSprite->GetProperty("tag") == "platform")
				{
					if (v0.m_x <= pSprite->GetLeft() + w)
					{
						m_player.SetVelocity(0, 0);
						m_player.SetX(pSprite->GetLeft() - w);
					}
				}
			}

		//right collision detection
		for (CSprite* pSprite : m_sprites)
			if (m_player.HitTest(pSprite, 0))
			{
				//platforms and walls
				if ((string)pSprite->GetProperty("tag") == "platform")
				{
					if (v0.m_x >= pSprite->GetRight() + w)
					{
						m_player.SetVelocity(0, 0);
						m_player.SetX(pSprite->GetRight() + w);
					}
				}
			}

		// Rope collision detection

		bool bTouchingLadder = false;
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "rope")
				{
					bTouchingLadder = true;
					m_player.SetVelocity(0, 0);
				}
			}
		}

		// Skull –collider collisions
		for (CSprite* pEnemy : m_sprites)
		{
			int w = pEnemy->GetWidth() / 2;
			if ((string)pEnemy->GetProperty("tag") == "enemy")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "lcollider" && pEnemy->HitTest(pCollider, 0))
					{
						//Collision response code –left collider
						if (v0.m_x <= pCollider->GetRight() + w)
							pEnemy->SetVelocity(0, 0);
						pEnemy->SetX(pCollider->GetRight() + w);
						pEnemy->Accelerate(10, 0);
						pEnemy->SetOmega(-3.82 * 100);
					}

				}

			if ((string)pEnemy->GetProperty("tag") == "enemy")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "rcollider" && pEnemy->HitTest(pCollider, 0))
					{
						//Collision response code –right collider
						if (v0.m_x >= pCollider->GetLeft() + w)
							pEnemy->SetVelocity(0, 0);
						pEnemy->SetX(pCollider->GetLeft() - w);
						pEnemy->Accelerate(-10, 0);
						pEnemy->SetOmega(-3.82 * 100);

					}

				}
		}

		// moving platform - colliders
		for (CSprite* pSprite : m_sprites)
		{
			int w = pSprite->GetHeight() / 2;
			if ((string)pSprite->GetProperty("tag") == "platform2")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "tcollider" && pSprite->HitTest(pCollider, 0))
					{
						//Collision response code –top collider
						if (v0.m_y <= pSprite->GetBottom() + w)
							pSprite->SetVelocity(0, 0);
						pSprite->SetY(pCollider->GetBottom() - w);
						pSprite->Accelerate(0, -30);

					}
				}

			if ((string)pSprite->GetProperty("tag") == "platform2")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "bcollider" && pSprite->HitTest(pCollider, 0))
					{
						//Collision response code –top collider
						if (v0.m_y >= pSprite->GetTop() + w)
							pSprite->SetVelocity(0, 0);
						pSprite->SetY(pCollider->GetTop() + w);
						pSprite->Accelerate(0, 30);

					}
				}
		}

		// collision detection between player and hazards
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "hazard")
				{
					m_player.Die(1000);
					m_player.SetImage("crouch_right");


				}
			}
		}

		// collision detection between player and enemies
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "enemy")
				{
					m_player.Die(1000);
					m_player.SetImage("crouch_right");


				}
			}
		}

		// collision detection between player and bananas

		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "banana")
				{
					pSprite->Delete();
					ban = 1;

				}
			}
		}

		// collision detection between player and signs
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "sign1")
				{

					if (ban == 1)
					{
						gamewon = true;
						levelpassed.Play("levelpassed.wav");
						NewLevel();
					}
					





				}
			}
		}

		// top collision detection between player and moving platforms
		bool bTouchingPlatform2 = false;
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "platform2")
				{
					if (v0.m_y >= pSprite->GetTop() + h)
					{

						m_state == FLYING;
						m_player.SetVelocity(0, 100);
						m_player.SetY(pSprite->GetTop() + h);
						bTouchingPlatform2 = true;
					}
				}
			}

			//bottom collision detection with moving platforms
			for (CSprite* pSprite : m_sprites)
				if (m_player.HitTest(pSprite, 0))
				{
					//platforms and walls
					if ((string)pSprite->GetProperty("tag") == "platform2")
					{
						if (v0.m_y <= pSprite->GetBottom() + h)
						{
							m_player.SetVelocity(0, 0);
						}
					}
				}



			if (m_state == AIRBORNE && bTouchingLadder)
			{
				// climbing
				m_state = CLIMBING;
				m_player.SetImage("hang");
			}

			if (m_state == CLIMBING)
			{
				if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP))
					m_player.Accelerate(0, 20);
				m_player.SetImage("climb");
			}

			if (m_state == CLIMBING && bTouchingPlatform)
			{
				m_state = STANDING;
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");

			}


			if (m_state == AIRBORNE && bTouchingPlatform)
			{
				//just landed
				m_state = STANDING;
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			}

			if (m_state != AIRBORNE && !bTouchingPlatform)
			{
				// just taken off
				m_state = AIRBORNE;
				m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
			}

			if (m_state == AIRBORNE && bTouchingPlatform2)
			{

				m_state = FLYING;
				m_player.Accelerate(0, 50);
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			}

		}
	}
}

		
         


void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		startScreen.Draw(g);
		return;
	}
	
	background.Draw(g);
	
	for (CSprite *pSprite : m_sprites)
		if ((string)pSprite->GetProperty("tag") != "rcollider" && (string)pSprite->GetProperty("tag") != "lcollider" && (string)pSprite->GetProperty("tag") != "tcollider" && (string)pSprite->GetProperty("tag") != "bcollider")
		pSprite->Draw(g);
	m_player.Draw(g);

	if (IsGameOverMode())
	{
		if (gamewon)
		{
			*g << font(46) << color(CColor::Green()) << vcenter << center << " GAME WON ";
		}
		else
		{
			*g << font(46) << color(CColor::Red()) << vcenter << center << " GAME OVER ";
		}
	}

	
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadImage("player.png", "stand_right", 8, 8, 0, 0, CColor::White());
	m_player.LoadImage("player.png", "stand_left", 8, 8, 0, 1, CColor::White());
	m_player.AddImage("player.png", "run_right",     8, 8, 6, 0, 6, 7, CColor::White());
	m_player.AddImage("player.png", "run_left",      8, 8, 5, 0, 5, 7, CColor::White());
	m_player.LoadImage("player.png", "jump_right",   8, 8, 4, 4, CColor::White());
	m_player.LoadImage("player.png", "jump_left",    8, 8, 3, 4, CColor::White());
	m_player.LoadImage("player.png", "crouch_right", 8, 8, 7, 4, CColor::White());
	m_player.LoadImage("player.png", "crouch_left",  11, 6, 2, 5, CColor::White());
	m_player.LoadImage("player.png", "hang",		 8, 8, 0, 5, CColor::White());
	m_player.AddImage("player.png", "climb",		 11, 6, 9, 2, 10, 2, CColor::White());
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	// default level

	gamewon = false;

	//clear sprites
	m_sprites.delete_all();

	if (level > 0)
		StartGame();
	


	/*StartGame();*/	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	//music to play continuously

	music.Play("music.wav", -1);
	
	SetLevel(level);
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CSprite *pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	CSprite *pSprite;

	switch (nLevel)
	{
	case 1:// build Level 1 sprites

		// spawn the player
		m_player.SetPos(790, 440);
		m_player.SetImage("stand_left");

		gamewon = false;

		level = 1;

		ban = 0;


		// spawn all other sprites here ...

		// platforms
		pSprite = new CSprite(400, 10, "tile1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(470, 90, "tile2.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(400, 200, "tile2.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-10, 270, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-30, 150, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(400, 330, "tile2.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(800, 400, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-40, 500, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(550, 480, "tile7.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(410, 480, "tile7.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(270, 480, "tile7.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		/*pSprite = new CSpriteRect(470, 120, 20, 800, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);*/

		// colliders
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(810, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		// rcollider
		pSprite = new CSpriteRect(790, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(150, 220, 20, 300, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		// lcollider
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);

		//ropes

		pSprite = new CSpriteRect(450,150,2,100, CColor::DarkGreen(), CColor::DarkGreen(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(40, 230, 2, 100, CColor::DarkGreen(), CColor::DarkGreen(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);


		// Enemies
		pSprite = new CSprite(20, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(20, 170, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		// Hazards
		pSprite = new CSprite(240, 30, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(420, 350, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		// Banana
		pSprite = new CSprite(20, 30, "banana.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "banana");
		m_sprites.push_back(pSprite);

		// Signs
		pSprite = new CSprite(750, 440, "Sign2.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign2");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(50, 540, "Sign1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign1");
		m_sprites.push_back(pSprite);


		break;

	case 2:// Level 2 (EMPTY)

	    // spawn the player
		m_player.SetPos(20, 560);
		m_player.SetImage("stand_right");

		gamewon = false;

		level = 2;

		ban = 0;
		
		   
		// platforms
		pSprite = new CSprite(400, 10, "tile1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(350, 200, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(350, 450, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-150, 500, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(860, 350, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(870, 500, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-130, 80, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		//moving platforms
		pSprite = new CSprite(150, 250, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform2");
		pSprite->SetVelocity(0, 100);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(550, 250, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform2");
		pSprite->SetVelocity(0, 100);
		m_sprites.push_back(pSprite);

		//top colliders
		pSprite = new CSpriteRect(0, 500, 5000, 30, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "tcollider");
		m_sprites.push_back(pSprite);

		//bottom colliders
		pSprite = new CSpriteRect(0, 70, 5000, 30, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "bcollider");
		m_sprites.push_back(pSprite);
		
		// Enemies
		pSprite = new CSprite(20, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(420, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(-100, 0);
		pSprite->SetOmega(-3.82 * 100);
		m_sprites.push_back(pSprite);

		// colliders
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(810, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		// rcollider
		pSprite = new CSpriteRect(790, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		// lcollider
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);

		// Banana
		pSprite = new CSprite(780, 30, "banana.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "banana");
		m_sprites.push_back(pSprite);

		// Hazards
		pSprite = new CSprite(240, 30, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(450, 30, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		// Signs
		pSprite = new CSprite(750, 540, "Sign1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign1");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(20, 540, "Sign3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign2");
		m_sprites.push_back(pSprite);

		// Trees
		pSprite = new CSprite(700, 60, "Tree1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "tree");
		m_sprites.push_back(pSprite);

		//ropes
		pSprite = new CSpriteRect(760, 450, 2, 100, CColor::DarkGreen(), CColor::DarkGreen(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);


		break;


		case 3:// Level 3 

	    // spawn the player
		m_player.SetPos(0, 540);
		m_player.SetImage("stand_right");

		gamewon = false;

		level = 3;

		ban = 0;

		// platforms
		pSprite = new CSprite(400, 10, "tile1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-120, 500, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(270, 470, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(-140, 350, "tile3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(470, 520, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(350, 300, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(810, 520, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(840, 100, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(820, 250, "tile4.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

        //top colliders
		pSprite = new CSpriteRect(0, 450, 5000, 30, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "tcollider");
		m_sprites.push_back(pSprite);

		//bottom colliders
		pSprite = new CSpriteRect(0, 130, 5000, 30, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "bcollider");
		m_sprites.push_back(pSprite);

		// colliders
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(810, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);
		
		// rcollider
		pSprite = new CSpriteRect(790, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rcollider");
		m_sprites.push_back(pSprite);

		// lcollider
		pSprite = new CSpriteRect(-10, 120, 20, 1000, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "lcollider");
		m_sprites.push_back(pSprite);

		//moving platforms
		pSprite = new CSprite(150, 250, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform2");
		pSprite->SetVelocity(0, 150);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(550, 250, "tile8.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform2");
		pSprite->SetVelocity(0, 150);
		m_sprites.push_back(pSprite);

		// Enemies
		pSprite = new CSprite(20, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(420, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(-100, 0);
		pSprite->SetOmega(-3.82 * 100);
		m_sprites.push_back(pSprite);

		// Banana
		pSprite = new CSprite(780, 30, "banana.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "banana");
		m_sprites.push_back(pSprite);

		//ropes
		pSprite = new CSpriteRect(760, 210, 2, 100, CColor::DarkGreen(), CColor::DarkGreen(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(40, 450, 2, 100, CColor::DarkGreen(), CColor::DarkGreen(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);

		// Hazards
		pSprite = new CSprite(240, 30, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(450, 30, "spikes.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "hazard");
		m_sprites.push_back(pSprite);

		// Signs
		pSprite = new CSprite(750, 570, "Sign1.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign1");
		m_sprites.push_back(pSprite);

		pSprite = new CSprite(20, 540, "Sign3.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "sign2");
		m_sprites.push_back(pSprite);




		break;
	}

	// any initialisation common to all levels
	m_player.SetImage("stand_right");
	m_player.SetVelocity(0, 0);
	m_state = STANDING;
	m_side = RIGHT;
}

// called when the game is over
void CMyGame::OnGameOver()
{	
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
	{
		level = 0;
		NewGame();
	}

	// dealing with menu selections when in Menu Mode
	if (IsMenuMode())
	{
		if (sym == SDLK_1)
		{
			level = 1;
			StartGame();
		}
		else if (sym == SDLK_2)
		{
			level = 2;
			StartGame();
		}
		else if (sym == SDLK_3)
		{
			level = 3;
			StartGame();
		}
	}
	if ((sym == SDLK_w || sym == SDLK_UP) && (m_state == STANDING || m_state == RUNNING))
	{
		m_player.Accelerate(0, 800);
		jumpsound.Play("jumpsound.wav");
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
			m_player.Accelerate(-300, 0);
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
			m_player.Accelerate(300, 0);
		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}
	
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
