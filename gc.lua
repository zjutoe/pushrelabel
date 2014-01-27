#!/usr/bin/env lua5.1

dofile(arg[1])

function show_cap(g)
   for u, l in pairs(g) do
      print(u)
      for v, c in pairs(l) do
	 print("", v, c)
      end
   end
end

function show_height(h)
   for i, v in pairs(h) do
      print(i, v)
   end
end

function show_flow(f)
   for u, l in pairs(f) do
      print(u)
      for v, x in pairs(l) do
	 print('', v, x)
      end
   end
end

function show_excess(e)
   for i, v in pairs(e) do
      print(i, v)
   end
end

show_cap(G)

function mset(m, i, j, v)
   if not m[i] then m[i]={} end
   if not m[i][j] then m[i][j]={} end
   m[i][j] = v
end

-- height
h={}
-- preflow
f={}
-- excess flow
e={}
-- residual flow
cf={}

function init_preflow(c)
   h['S'] = #c

   for v, x in pairs(c.S) do      
      mset(f, 'S', v, x)
      mset(f, v, 'S', -x)
      e[v] = x
      e['S'] = (e['S'] or 0) - x
      mset(cf, 'S', v, x - f['S'][v])
      mset(cf, v, 'S', -x - f[v]['S'])
   end
end

init_preflow(G)

print("Height:")
show_height(h)
print("Preflow:")
show_flow(f)
print("Excess:")
show_excess(e)


function push(u, v)
   local temp = (e[u] < cf[u][v]) and e[u] or cf[u][v]
   mset(f, u, v, f[u][v] + temp)
   mset(f, v, u, -f[u][v])
   e[u] = e[u] - temp
   e[v] = e[v] + temp
   mset(cf, u, v, c[u][v] - f[u][v])
   mset(cf, v, u, c[v][u] - f[v][u])
end

function relabel(c, u)
   local temp = -1
   
   for v, x in pairs(c[u]) do
      if cf[u][v] > 0 then
	 if temp == -1 or temp > h[v] then
	    temp = h[v]
	 end	 
      end
   end

   h[u] = 1 + temp   
end

