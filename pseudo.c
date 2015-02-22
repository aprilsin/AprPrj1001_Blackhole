main()
{
	for(;;)
	{
		stateWait();
		stateList();
		stateListPost();
		stateStar();
		stateStarPost();
		Store "Yes" or "No" to matchKey;
		stateMatch(matchKey);
	}
}

stateWait()
{
	while (!ListKeyOn());
}

stateList()
{
	while (ListKeyOn())
	{
		listDisplayRandom();
	}
}

stateListPost()
{
	listDisplayRandomSlow();
	storeLastList();
	while (!StarKeyOn());
}

stateStar()
{
	while (StarKeyOn())
	{
		starDisplayRandom();
	}
}

stateStarPost()
{
	starDisplayRandomSlow();
	storeLastStar();
	while (!MatchKeyOn());
}

stateMatch(matchKey)
{
	If ((( Match(List, Star) && (matchKey == "Yes")) ||
	    ((!Match(List, Star) && (matchKey == "No" ))) 	Win()
	else							Lose();
	
	while (!resetKeyOn())
	{
		blink list;
		blink graph;
	};
}

win()
{
	Play winning MP3;
}

lose()
{
	Play "boo boo" sound efx;
}
