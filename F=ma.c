/***************************************/
/* Do a descrete simulation using F=ma */
/* over a time series                  */
/***************************************/

double bodydia = 0.010 ;  # diameter of rocket body
double chutedia = 0.100;
double mr  = 0.050  ; # mass of rocket  in kg

double T   = 4.5 ;       # Thrust of motor in Newtons
double t   = 2;          # thrust burn time
double mp  = 0.003 * t;  # propellant in grams per second

double delay = 23; # number of seconds from start of flight to chute deployment

double g   = 9.81; # gravitational constant meter/sec**2
double rho = 1.22; # density of air at sea level kg/meter**3 (-10% per 1000 feet)
double Cd  = 0.75; # coeficient of drag for most rockets (no units)
double chuteCd = 1.5; # Cd for a hemisphere chute, Tethered sheets are lower.
double pi  = 3.14159; #

double bodyA  =  pi*(bodydia/2)^2 ; # frontal area in meters**2 ignore fin area.
double chuteA =  pi*(chutedia/2)^2 ; #  chute area in meters**2

# Boost phase
# Assume that propellent generates flat thrust curve

double Fthrust (double vel, double now) {
    if (now < t) {
	return T;
    }
    else {
	return 0.0;
    }
} 

double Fbodydrag {
    my $vel = shift;
    my $now = shift;

    my $sign = ($vel > 0)? -1:1;

    return $sign * 1/2 * $rho * $Cd * $bodyA * $vel**2;
} 

sub Fchutedrag {
    my $vel = shift;
    my $now = shift;
    my $opentime = 2; # how fast does the chute open in sec

    my $sign = ($vel > 0)? -1:1;

    if ($now > $delay) {
	if ($now < ($delay + $opentime)) {
	    return $sign * 1/2 * $rho * $chuteCd * $chuteA*($now-$delay)/$opentime * $vel**2;
	} 
	    return $sign * 1/2 * $rho * $chuteCd * $chuteA * $vel**2;
	}
    else {
	return 0;
    } 
} 

sub Fgravity {
    my $vel = shift;
    my $now = shift;

    return Mass($vel, $now) * $g;
}  

sub Mass  {
    my $vel = shift;
    my $now = shift;
    return ($mr + ($mp/$t)*($t-$now)) if($now < $t);
    return  $mr;
} 

my $now = 0.0000;
my $delta = 0.01;
my $alt =  0;
my $vel = 0;

while ( $alt >= 0 ) {
    
    my ($Ft, $Fbd, $Fg, $Fcd, $M) = (
	Fthrust($vel, $now), Fbodydrag($vel, $now),
	Fgravity($vel, $now), Fchutedrag($vel, $now),
	Mass($vel, $now)
    );

    my $acc = ($Ft - $Fg + $Fbd + $Fcd)/ $M;

    $vel += $acc * $delta;
    $alt += $vel * $delta;

    printf "%9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f %9.4f\n",
	$now, $acc, $vel, $alt, $Ft, $Fbd, $Fg, $Fcd, $M;

    $now += $delta;
}
