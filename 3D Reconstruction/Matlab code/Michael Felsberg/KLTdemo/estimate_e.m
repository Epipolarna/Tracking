function e = estimate_e(Ig, Jg, dx, dy, x, y, xradius, yradius)


dx = dx(y-yradius:y+yradius, x-xradius:x+xradius);
dy = dy(y-yradius:y+yradius, x-xradius:x+xradius);
Ig = Ig(y-yradius:y+yradius, x-xradius:x+xradius);
Jg = Jg(y-yradius:y+yradius, x-xradius:x+xradius);

e1 = (Ig - Jg).*dx;
e2 = (Ig - Jg).*dy;

e1 = sum(e1(:));
e2 = sum(e2(:));

e = [e1;
     e2];