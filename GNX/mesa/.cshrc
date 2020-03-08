if ( $?prompt ) then
	set history = 100
	if ( $user != root ) then
		set oldprompt = $prompt
		source ~$user/.cshrc
		set prompt = "$oldprompt $prompt"
		unset oldprompt
	endif
endif
