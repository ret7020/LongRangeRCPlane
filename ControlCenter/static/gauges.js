var panel = g3.panel()
    .width(1200).height(400)
    .append(
        g3.put().x(200).y(200).scale(1.5).append(g3.contrib.nav.attitude.generic()),
        g3.put().x(550).y(200).scale(1.5).append(g3.contrib.nav.heading.generic()),
        g3.put().x(900).y(200).scale(1.5).append(g3.contrib.clocks.simple()),
    );

panel.interval(250).url('/api/debug/metrics');
panel('#gauge');