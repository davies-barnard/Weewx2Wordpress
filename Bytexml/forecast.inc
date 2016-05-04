## forecast display for weewx - Copyright 2012-2013 Matthew Wall
## $Id: forecast.inc 710 2013-11-09 15:42:51Z mwall $
## Include this file to display forecast data.	This file uses icons from the
## forecast icon set.

## version = 0.1

#if $varExists('forecast')

	## Which forecast should be displayed?	options are WU or NWS
	#set $forecast_source = 'WU'

	## How many forecast periods should be considered?
	#set $num_periods = 40

	## These are the styles used by the forecast display.	 Putting the styles
	## like this works (in page, not in head), but it is not good practice.

	<forecast status="true">

	## Get any tide predictions
	#set $tides = $forecast.xtides

	<legend>
		<disclosure></disclosure>
		<date></date>
		<outlook></outlook>
		<temp icon="thermometer-red.png" alt='T' title='temperature'>$unit.label.outTemp</temp>
		<dewpoint icon="thermometer-dewpoint.png" alt='D' title='dewpoint'>$unit.label.dewpoint</dewpoint>
		<humidity icon="thermometer-blue.png" alt='H' title='humidity'>$unit.label.outHumidity</humidity>
		<wind icon="flag.png" alt='W' title='wind'>$unit.label.wind</wind>
		<tides icon="water.png" alt='tide' title='tides'>$unit.label.altitude</tides>
		<sun icon='sunriseset.png' alt='S' title='sun'></sun>
		<moon icon="moonriseset.png" alt='M' title='moon'></moon>
		<moonphase icon="moonphase.png" alt='MP' title='moon phase'></moonphase>
		<pop icon="pop.png" alt='PoP' title='probability of precipitation'>$unit.label.rain</pop>
		<precip></precip>
		<obvis></obvis>
	</legend>


	## Some forecasts return 16-points of the compass, but the icons support
	## only 8-points.	 This dictionary maps the 16 points to 8 points.
	#set $wind_dir_dict = {'SSW':'SW','WSW':'SW','NNW':'NW','WNW':'NW','SSE':'SE','ESE':'SE','NNE':'NE','ENE':'NE'}

	#set $lastday = None
	#set $periods = $forecast.weather_periods($forecast_source, max_events=$num_periods)

	#for $period in $periods

		#set $thisday = $period.event_ts.format('%d')
		#set $thisdate = $period.event_ts.format('%Y.%m.%d')
		#set $smryid = $forecast_source + '.' + $thisdate
		#set $hourid = $forecast_source + '.' + $thisdate + '.hours'
		#set $alm = $forecast.almanac(ts=$period.event_ts.raw)
		#set $nextday_ts = time.mktime(time.strptime($thisdate + ' 00:00', '%Y.%m.%d %H:%M')) + 24*3600

		#if $alm.hasExtras
			#set $sunrise_ts = $alm.sunrise.raw
			#set $sunset_ts = $alm.sunset.raw
			#set $moonrise_ts = $alm.moon.rise.raw
			#set $moonset_ts = $alm.moon.set.raw
		#else
			## non-pyephem sunrise/sunset is not a ValueHelper so do conversions
			#set $sunrise_ts = time.mktime(time.strptime($thisdate + ' ' + $alm.sunrise, '%Y.%m.%d %H:%M'))
			#set $sunset_ts = time.mktime(time.strptime($thisdate + ' ' + $alm.sunset, '%Y.%m.%d %H:%M'))
		#end if

		#set $wts = $period.issued_ts

		#if $lastday != $thisday

			#set $lastday = $thisday
			#set $summary = $forecast.weather_summary($forecast_source, $period.event_ts.raw, periods=$periods)
			#set $wloc = $summary.location

			<summary id='$smryid'>

				<day>$summary.event_ts.format('%a')</day>
				<date>$summary.event_ts.format('%d %b %Y')</date>

				#if $summary.clouds is not None
					#set $simg = $summary.clouds + '.png'
					<outlook icon='$simg' alt='$summary.clouds'>$forecast.label('Weather', $summary.clouds)</outlook>
				#end if

				<temphi>$summary.tempMax.nolabel('%.0f')</temphi>
				<templo>$summary.tempMin.nolabel('%.0f')</templo>
				<dewpointMax>$summary.dewpointMax.nolabel('%.0f')</dewpointMax>
				<dewpointMin>$summary.dewpointMin.nolabel('%.0f')</dewpointMin>
				<humidityMax>$summary.humidityMax.nolabel('%.0f',' ')</humidityMax>
				<humidityMin>$summary.humidityMin.nolabel('%.0f',' ')</humidityMin>
				
				<windDir>
					#for $d in $summary.windDirs
						<event alt="$d">$wind_dir_dict.get($d,$d)</event>
					#end for
				</windDir>
				<windSpd>
					#if $summary.windSpeedMin == $summary.windSpeedMax or $summary.windSpeedMin.raw == $summary.windSpeedMax.raw
								$summary.windSpeedMin.nolabel('%.0f',' ')
					#else
								$summary.windSpeedMin.nolabel('%.0f',' ')-$summary.windSpeedMax.nolabel('%.0f',' ')
					#end if
				</windSpd>
				<windGust>$summary.windGust.nolabel('%.0f',' ')</windGust>
				<windChar>$summary.windChar</windChar>


				#if len($tides) > 0
					<tides>
						#for $tide in $tides
							#if $summary.event_ts.raw <= $tide.event_ts.raw < $summary.event_ts.raw + $summary.duration
								<event time="$tide.event_ts.format('%H:%M')" status="$tide.hilo">$tide.offset.nolabel('%.1f')</event>
							#end if
						#end for
					</tides>
				#end if

				<sun>
					#if $alm.hasExtras
						<rise>$alm.sun.rise</rise>
						<set>$alm.sun.set</set>
					#else
						<rise>$alm.sunrise</rise>
						<set>$alm.sunset</set>
					#end if
				</sun>

				<moon>
					#if $alm.hasExtras
						<rise>$alm.moon.rise</rise>
						<set>$alm.moon.set</set>
					#end if
					<phase>$alm.moon_fullness%</phase>
				</moon>

				<pop>
					#if $summary.pop.raw > 0
						<chance>$summary.pop.format('%.0f','')</chance>
						#if $summary.qpf.raw > 0
							<rain icon="raindrop.png">$summary.qpf.nolabel('%.2f','')</rain>
						#end if
						#if $summary.qsf.raw > 0
							<snow icon="snowflake.png">$summary.qsf.nolabel('%.2f','')</snow>
						#end if
					#end if
				</pop>

				<precip>
					#for $k in $summary.precip
						#set $precip_img = $k + '.png'
						<event icon='$precip_img' alt='$k'>$forecast.label('Weather', $k)</event>
					#end for
				</precip>
				<obvis>
					#for $k in $summary.obvis
						#set $obvis_img = $k + '.png'
						<event icon='$obvis_img' alt='$k'>$forecast.label('Weather', $k)</event>
					#end for
				</obvis>

			</summary>
			
		#end if ## lastday != thisday

		<period id='$hourid' class='period'>

			<day>$period.event_ts.format('%a')</day>
			<date>$period.event_ts.format('%d %b %Y')</date>	
			<hour>$period.event_ts.format('%H:%M')</hour>
			
			#if $period.clouds is not None
				#if $sunrise_ts < $period.event_ts.raw < $sunset_ts
					#set $outlook_img = $period.clouds + '.png'
				#else
					#set $outlook_img = $period.clouds + 'n.png'
				#end if
				<outlook icon='$outlook_img' alt='$period.clouds' title='$forecast.label('Weather', $period.clouds)'></outlook>
			#end if

			<temp>$period.temp.nolabel('%.0f')</temp>
			<dewpoint>$period.dewpoint.nolabel('%.0f')</dewpoint>
			<humidity>$period.humidity.nolabel('%.0f',' ')</humidity>
			<wind icon="$wind_dir_dict.get($period.windDir,$period.windDir)">$period.windDir</wind>
			<windSpd>$period.windSpeed.nolabel('%.0f',' ')</windSpd>
			<windGust>$period.windGust.nolabel('%.0f',' ')</windGust>
			<windChar>$period.windChar</windChar>

			#if len($tides) > 0
				<tides>
					#for $tide in $tides
						#if $period.event_ts.raw <= $tide.event_ts.raw < $period.event_ts.raw + $period.duration and $tide.event_ts.raw < $nextday_ts
							<event time="$tide.event_ts.format('%H:%M')" status="$tide.hilo">$tide.offset.nolabel('%.1f')</event>
						#end if
					#end for
				</tides>
			#end if

			<sun>
				#if $period.event_ts.raw <= $sunrise_ts < $period.event_ts.raw + $period.duration
					<rise>$alm.sunrise</rise>
				#end if

				#if $period.event_ts.raw <= $sunset_ts < $period.event_ts.raw + $period.duration
					<set>$alm.sunset</set>
				#end if
			</sun>

			#if $alm.hasExtras
				<moon>
					#if $period.event_ts.raw <= $moonrise_ts < $period.event_ts.raw + $period.duration
						<rise>$alm.moon.rise</rise>
					#end if
					#if $period.event_ts.raw <= $moonset_ts < $period.event_ts.raw + $period.duration
						<set>$alm.moon.set</set>
					#end if
				</moon>
			#end if

			<pop>
				#if $period.pop.raw > 0
					<chance>$period.pop.format('%.0f',' ')</chance>
					#if $period.qpf.raw > 0
						<rain icon="raindrop.png">$period.qpf.nolabel('%.2f',' ')</rain>
					#end if
					#if $period.qsf.raw > 0
						<snow icon="snowflake.png">$period.qsf.nolabel('%.2f',' ')</snow>
					#end if
				#end if
			</pop>

			<precip>
				#for $k,$v in $period.precip.items()
					#set $precip_img = $k + '.png'
					#if $v != ''
						#set $precip_lbl = $forecast.label('Weather', $v+'q')
					#else
						#set $precip_lbl = ""
					#end if
					<event icon='$precip_img' alt='$k'>$forecast.label('Weather', $k)</event>
				#end for
			</precip>

			#if $period.obvis is not None
				#set $obvis_img = $period.obvis + '.png'
				<obvis icon='$obvis_img' alt='$period.obvis' title='$forecast.label('Weather', $period.obvis)'></obvis>
			#end if

		</period>

	#end for ## period in periods

	<sources>
		#if len($tides) > 0
			<tides location="$tides[0].location">$tides[0].issued_ts<tides>
		#end if
		#if $wloc is not None and $wloc != ''
			#set $location=$wloc
		#else
			#set $location="unknown"
		#end if
		<forecast source="$forecast_source" location="$location">$wts</forecast>
	</sources>

</forecast>

#else
<forecast status="false">forecasting is not installed</forecast>
#end if