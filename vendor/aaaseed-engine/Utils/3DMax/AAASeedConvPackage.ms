-- Récupèration en absolu des coordonnées des géométrie
-- Tristan Lorach 1999
-- Maa 1999
-- phillipe lamoureux 1999
------------------------------------------------------------------
-- ConvPackage
-- Reprend les trois scripts de conversion en une interface unique
------------------------------------------------------------------
-- Rollout FonctionsR en global pour pouvoir accéder à ses éléments
global FonctionsR
------------------------------------------------------------------
-- Script select_conv
-- 	(Les quelques modifications apportées sont précédées du
-- 	commentaire '-- Modif')
------------------------------------------------------------------


-- ***************************
-- *** Load Selection	   ***
-- ***************************
fn load_selection fname =
(
	-- Modif
	-- Efface d'abord la sélection courante
	ClearSelection()

  fd2 = openfile fname mode:"r"
  cnt = ( readline fd2 ) as number
  for i = 1 to cnt do
  (
    sname = readline fd2
	for geom in $geometry/* do 
	( 
		if geom.name == sname do 
		( 
			selectmore geom 
		) 
	)
  )
  close fd2
)

------------------------------------------------------------------
-- Script select_save
-----------------------------------------------------------------

-- ***************************
-- *** Save Selection	   ***
-- ***************************
fn save_selection fname =
(
  fd = createfile fname
  format "%\n" selection.count to:fd
  for geom in $selection/* do
  (
    format "%\n" geom.name to:fd
  )
  close fd
)

-- ***************************
-- *** ECRITURE EN MATRICE ***
-- ***************************
fn write_mat transf fd =
(
	-- Ecriture de la matrice homogène:
	format "%\t%\t%\t0\n" transf.row1.x transf.row1.y transf.row1.z to:fd
	format "%\t%\t%\t0\n" transf.row2.x transf.row2.y transf.row2.z to:fd
	format "%\t%\t%\t0\n" transf.row3.x transf.row3.y transf.row3.z to:fd
	format "%\t%\t%\t1\n" transf.row4.x transf.row4.y transf.row4.z to:fd
)
-- ***************************
-- ***  EULER			   ***
-- ***************************
fn write_euler transf fd =
(
	rot = ( inverse transf.rotationpart ) as EulerAngles
	tra = transf.translationpart
	sca = transf.scalepart
	-- Ecriture des transformations:
	format "Rot % % % Tra % % % Sca % % %\n" rot.x rot.y rot.z tra.x tra.y tra.z sca.x sca.y sca.z to:fd
)
-- ***************************
-- ***  QUATERNIONS		   ***
-- ***************************
fn write_quat transf fd =
(
	q = ( inverse transf.rotationpart ) as Quat
	q = inverse q -- histoire de règles de main droite (cf doc)
	tra = transf.translationpart
	sc =  transf.scalepart
	-- Ecriture des transformations:
	format "Quat % % % %\n" q.w q.x q.y q.z to:fd
	format "Tra % % %\n" tra.x tra.y tra.z to:fd
	format "Sc  % % %\n" sc.x sc.y sc.z to:fd
)
-- ***************************
-- ***  ANGLE+AXIS		   ***
-- ***************************
fn write_axis transf fd =
(
	ax = ( inverse transf.rotationpart ) as AngleAxis
	tra = transf.translationpart
	sc =  transf.scalepart
	-- Ecriture des transformations:
	format "AngleAxis % % % %\n" ax.angle ax.axis.x ax.axis.y ax.axis.z to:fd
	format "Tra % % %\n" tra.x tra.y tra.z to:fd
	format "Sc  % % %\n" sc.x sc.y sc.z to:fd
)

-- ne marche pas :
-- for slidertime = animationrange.start to animationrange.end do

-- Placé dans l'interface
	-- ********* selection automatique
	-- load_selection "convert_sel.txt"

------------------------------------------------------
-- WriteTransforms
-- Ecriture des transforms placée dans cette fonction
------------------------------------------------------
fn WriteTransforms fname = 
(
-- Modif
-- Placé dans l'init globale
	-- ********* toute manip se fait selon le 'world'
	coordsys world

	-- ********* Crée un fichier
	-- openlog fname mode:"w" outputonly:true
	fd = createfile fname

	if ( FonctionsR.b_loop.state )
	then
		end_save = animationrange.end - 1
	else
		end_save = animationrange.end
	-- ********* Ecrire des infos générales
	format " frame_start %" animationrange.start to:fd
	seek fd ((filepos fd) - 1) -- get rid of the trailing f on float
	format " frame_end %" end_save to:fd
	seek fd ((filepos fd) - 1) -- get rid of the trailing f on float
	format " nodes %\n" $selection.count to:fd
	
	counter = 0
	for geom in $selection/* do
		(
	    format "Node % \"%\"\n" counter geom.name to:fd
		counter += 1
		)
		
	-- ********* parcourir l'animation (pas trouvé d'autre méthode)
	-- for frame = 1 to 1 do
	for frame = animationrange.start to end_save do
		(
		-- On change l'animation
		slidertime = frame
		  
		-- On écrit le numéro de frame
		format "\nframe %" frame to:fd
		seek fd ((filepos fd) - 1) -- get rid of the trailing f on float
		format " \n" to:fd

		  
		-- On s'occupe de toutes les geometries
		counter = 0
		
		case FonctionsR.RbType.state of
			(
			1 :
				for geom in $selection/* do
					(
				    transf = geom.transform
					write_mat transf fd
					counter += 1
					)
			2 :
				if ( FonctionsR.b_tra.state and FonctionsR.b_rot.state and FonctionsR.b_sca.state )
				then
					(
					for geom in $selection/* do
						(
						transf = geom.transform
							(
							-- write_euler transf fd
							rot = ( inverse transf.rotationpart ) as EulerAngles
							tra = transf.translationpart
							sca =  transf.scalepart
							-- Ecriture des transformations:
							format "Rot % % % Tra % % % Sca % % %\n" rot.x rot.y rot.z tra.x tra.y tra.z sca.x sca.y sca.z to:fd
							)
						counter += 1
						)
					)
				else if ( not FonctionsR.b_rot.state and not FonctionsR.b_sca.state )
				then
					(
					for geom in $selection/* do
						(
						transf = geom.transform
							(
							-- write_euler transf fd
							tra = transf.translationpart
							-- Ecriture des transformations:
							format "Tra % % %\n" tra.x tra.y tra.z to:fd
							)
						counter += 1
						)
					)
				else
					(
					messageBox "seulement translation ou tout implémenté.\nfichier invalide créé."
					exit
					)
			3 : 
				for geom in $selection/* do
					(
				    transf = geom.transform
					write_quat transf fd
					counter += 1
					)
			4 : 
				for geom in $selection/* do
					(
				    transf = geom.transform
					write_axis transf fd
					counter += 1
					)
			)
		)
	-- flushlog()
	-- closelog()
	close fd
	messageBox "Conversion Terminée !\nUn tour de moulin à prière\n pour Tristan Philippe et Mâa."
)	

------------------------------------------------------------------
-- Définition de l'interface
------------------------------------------------------------------
Rollout FonctionsR "Fonctions" (
	Button BtLoadSel "Charger une sélection" width:150
	Button BtSaveSel "Sauver la sélection actuelle" width:150
	Label Lb01 "Format d'écriture" across:2
	RadioButtons RbType "" labels:#("Matrice","Euler","Quaternions","Angle && Axis") default:2 columns:1 align:#LEFT 
	checkbox b_tra "Translation" checked:true
	checkbox b_rot "Rotation" checked:true
	checkbox b_sca "Scaling" checked:true
	Button BtWrite "Ecriture conversions" width:150
	checkbox b_loop "Loop" checked:true

	on BtLoadSel pressed do (
		-- Modif
		-- Utilise getOpenFileName et non getSaveFileName
		fname = getOpenFileName caption:"Load selection" filename:maxFileName types:"*.txt|*.txt"
		if (fname != undefined) then
			load_selection fname
		)
	on BtSaveSel pressed do (
		-- ********* déterminer un fichier de sauvegarde
		fname = getSaveFileName caption:"Save selection" filename:maxFileName types:"*.txt|*.txt"
		if (fname != undefined) then		
			save_selection fname
		)
	on BtWrite pressed do (
		-- ********* déterminer un fichier de sauvegarde
		fname = getSaveFileName caption:"Save conversion" filename:maxFileName types:"*.txt|*.mocap.maa|*.*"
		-- Si le fichier a été choisi, écriture
		if (fname != undefined) then
			WriteTransforms fname
		)
	
)


------------------------------------------------------------------

-- ********* toute manip se fait selon le 'world'
coordsys world

-- Fermeture du floater si il existe déjà
if (ConvFloater!=undefined) then CloseRolloutFloater ConvFloater
-- Création du floater
ConvFloater = NewRolloutFloater "Conversion" 230 212
-- Ajout d'un rollout (obligatoire) devant contenir les fonctions
AddRollout FonctionsR ConvFloater
