dofile("test.txt")

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

   for v, x in c.S do      
      mset(f, 'S', v, x)
      mset(f, v, 'S', -v)
      e[v] = x
      e['S'] = (e['S'] or 0) - v
      mset(cf, 'S', v, x - f['S'][v])
      mset(cf, v, 'S', -v - f[v]['S'])
   end
end

function push(u v)
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

