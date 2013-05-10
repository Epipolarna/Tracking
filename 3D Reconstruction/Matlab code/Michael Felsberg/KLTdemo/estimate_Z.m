function Z = estimate_Z(dx, dy, x, y, xradius, yradius)

dx = dx(y-yradius:y+yradius, x-xradius:x+xradius);
dy = dy(y-yradius:y+yradius, x-xradius:x+xradius);

z11 = dx.^2;
z12 = dy.*dx;
z22 = dy.^2;

z11 = sum(z11(:));
z12 = sum(z12(:));
z22 = sum(z22(:));

Z = [z11 z12;
     z12 z22];