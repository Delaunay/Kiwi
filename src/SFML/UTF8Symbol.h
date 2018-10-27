#pragma once

namespace kiwi{

/*
#define UTF8_SYMBOLS\
    // Misc Symbols
    UTF8Symbol(BLACK STAR, black_star, 2605, ★),\
    UTF8Symbol(WHITE STAR, white_star, 2606,  ☆),\
    UTF8Symbol(LIGHTNING, curved_bot_arrow, 2607, ☇),\
    UTF8Symbol(THUNDERSTORM, 2608, ☈),\
    UTF8Symbol(SUN, circle_dot, 2609, ),\
    UTF8Symbol(BALLOT BOX, 2610, ☐),\
    UTF8Symbol(BALLOT BOX WITH CHECK, 2611, ☑),\
    UTF8Symbol(BALLOT BOX WITH X, 2612, ☒),\
    UTF8Symbol(SKULL AND CROSSBONES, 2620, ☠),\

☢	9762	2622	 	RADIOACTIVE SIGN
☣	9763	2623	 	BIOHAZARD SIGN
☮	9774	262E	 	PEACE SYMBOL
☯	9775	262F	 	YIN YANG
☹	9785	2639	 	WHITE FROWNING FACE
☺	9786	263A	 	WHITE SMILING FACE
☼	9788	263C	 	WHITE SUN WITH RAYS
☽	9789	263D	 	FIRST QUARTER MOON
☾	9790	263E	 	LAST QUARTER MOON
♲	9842	2672	 	UNIVERSAL RECYCLING SYMBOL
♻	9851	267B	 	BLACK UNIVERSAL RECYCLING SYMBOL
♾	9854	267E	 	PERMANENT PAPER SIGN
℃	8451	2103	 	DEGREE CELSIUS
// Letter Like
ℕ	8469	2115	 	DOUBLE-STRUCK CAPITAL N
ℙ	8473	2119	 	DOUBLE-STRUCK CAPITAL P
ℚ	8474	211A	 	DOUBLE-STRUCK CAPITAL Q
ℝ	8477	211D	 	DOUBLE-STRUCK CAPITAL R
ℤ	8484	2124	 	DOUBLE-STRUCK CAPITAL Z
ℾ	8510	213E	 	DOUBLE-STRUCK CAPITAL GAMMA
ℿ	8511	213F	 	DOUBLE-STRUCK CAPITAL PI
⅀	8512	2140	 	DOUBLE-STRUCK N-ARY SUMMATION
ℼ	8508	213C	 	DOUBLE-STRUCK SMALL PI
ℯ	8495	212F	 	SCRIPT SMALL E
// Arrows
↑	8593	2191	&uarr;	UPWARDS ARROW
↓	8595	2193	&darr;	DOWNWARDS ARROW
↜	8604	219C	 	LEFTWARDS WAVE ARROW
↝	8605	219D	 	RIGHTWARDS WAVE ARROW
↩	8617	21A9	 	LEFTWARDS ARROW WITH HOOK
↰	8624	21B0	 	UPWARDS ARROW WITH TIP LEFTWARDS
↱	8625	21B1	 	UPWARDS ARROW WITH TIP RIGHTWARDS
↲	8626	21B2	 	DOWNWARDS ARROW WITH TIP LEFTWARDS
↳	8627	21B3	 	DOWNWARDS ARROW WITH TIP RIGHTWARDS
↴	8628	21B4	 	RIGHTWARDS ARROW WITH CORNER DOWNWARDS
↵	8629	21B5	&crarr;	DOWNWARDS ARROW WITH CORNER LEFTWARDS
↶	8630	21B6	 	ANTICLOCKWISE TOP SEMICIRCLE ARROW
↷	8631	21B7	 	CLOCKWISE TOP SEMICIRCLE ARROW
↸	8632	21B8	 	NORTH WEST ARROW TO LONG BAR
↹	8633	21B9	 	LEFTWARDS ARROW TO BAR OVER RIGHTWARDS ARROW TO BAR
↺	8634	21BA	 	ANTICLOCKWISE OPEN CIRCLE ARROW
↻	8635	21BB	 	CLOCKWISE OPEN CIRCLE ARROW
↪	8618	21AA	 	RIGHTWARDS ARROW WITH HOOK
⇄	8644	21C4	 	RIGHTWARDS ARROW OVER LEFTWARDS ARROW
⇅	8645	21C5	 	UPWARDS ARROW LEFTWARDS OF DOWNWARDS ARROW
⇆	8646	21C6	 	LEFTWARDS ARROW OVER RIGHTWARDS ARROW
⇇	8647	21C7	 	LEFTWARDS PAIRED ARROWS
⇈	8648	21C8	 	UPWARDS PAIRED ARROWS
⇉	8649	21C9	 	RIGHTWARDS PAIRED ARROWS
⇊	8650	21CA	 	DOWNWARDS PAIRED ARROWS
⇋	8651	21CB	 	LEFTWARDS HARPOON OVER RIGHTWARDS HARPOON
⇌	8652	21CC	 	RIGHTWARDS HARPOON OVER LEFTWARDS HARPOON
⇍	8653	21CD	 	LEFTWARDS DOUBLE ARROW WITH STROKE
⇎	8654	21CE	 	LEFT RIGHT DOUBLE ARROW WITH STROKE
⇏	8655	21CF	 	RIGHTWARDS DOUBLE ARROW WITH STROKE
⇐	8656	21D0	&lArr;	LEFTWARDS DOUBLE ARROW
⇑	8657	21D1	&uArr;	UPWARDS DOUBLE ARROW
⇒	8658	21D2	&rArr;	RIGHTWARDS DOUBLE ARROW
⇓	8659	21D3	&dArr;	DOWNWARDS DOUBLE ARROW
⇔	8660	21D4	&hArr;	LEFT RIGHT DOUBLE ARROW
⇕	8661	21D5	 	UP DOWN DOUBLE ARROW
⇖	8662	21D6	 	NORTH WEST DOUBLE ARROW
⇗	8663	21D7	 	NORTH EAST DOUBLE ARROW
⇘	8664	21D8	 	SOUTH EAST DOUBLE ARROW
⇙	8665	21D9	 	SOUTH WEST DOUBLE ARROW
⇚	8666	21DA	 	LEFTWARDS TRIPLE ARROW
⇛	8667	21DB	 	RIGHTWARDS TRIPLE ARROW


// Math
∀	8704	2200	&forall;	FOR ALL
∁	8705	2201	 	COMPLEMENT
∂	8706	2202	&part;	PARTIAL DIFFERENTIAL
∃	8707	2203	&exist;	THERE EXISTS
∄	8708	2204	 	THERE DOES NOT EXIST
∅	8709	2205	&empty;	EMPTY SET
∆	8710	2206	 	INCREMENT
∇	8711	2207	&nabla;	NABLA
∈	8712	2208	&isin;	ELEMENT OF
∉	8713	2209	&notin;	NOT AN ELEMENT OF
∊	8714	220A	 	SMALL ELEMENT OF
∋	8715	220B	&ni;	CONTAINS AS MEMBER
∌	8716	220C	 	DOES NOT CONTAIN AS MEMBER
∍	8717	220D	 	SMALL CONTAINS AS MEMBER
∎	8718	220E	 	END OF PROOF
∏	8719	220F	&prod;	N-ARY PRODUCT
∐	8720	2210	 	N-ARY COPRODUCT
∑	8721	2211	&sum;	N-ARY SUMMATION
−	8722	2212	&minus;	MINUS SIGN
∓	8723	2213	 	MINUS-OR-PLUS SIGN
∔	8724	2214	 	DOT PLUS
∕	8725	2215	 	DIVISION SLASH
∖	8726	2216	 	SET MINUS
∗	8727	2217	&lowast;	ASTERISK OPERATOR
∘	8728	2218	 	RING OPERATOR
∙	8729	2219	 	BULLET OPERATOR
√	8730	221A	&radic;	SQUARE ROOT
∛	8731	221B	 	CUBE ROOT
∜	8732	221C	 	FOURTH ROOT
∝	8733	221D	&prop;	PROPORTIONAL TO
∞	8734	221E	&infin;	INFINITY
∟	8735	221F	 	RIGHT ANGLE
∠	8736	2220	&ang;	ANGLE
∡	8737	2221	 	MEASURED ANGLE
∢	8738	2222	 	SPHERICAL ANGLE
∣	8739	2223	 	DIVIDES
∤	8740	2224	 	DOES NOT DIVIDE
∥	8741	2225	 	PARALLEL TO
∦	8742	2226	 	NOT PARALLEL TO
∧	8743	2227	&and;	LOGICAL AND
∨	8744	2228	&or;	LOGICAL OR
∩	8745	2229	&cap;	INTERSECTION
∪	8746	222A	&cup;	UNION
∫	8747	222B	&int;	INTEGRAL
∬	8748	222C	 	DOUBLE INTEGRAL
∭	8749	222D	 	TRIPLE INTEGRAL
∮	8750	222E	 	CONTOUR INTEGRAL
∯	8751	222F	 	SURFACE INTEGRAL
∰	8752	2230	 	VOLUME INTEGRAL
∱	8753	2231	 	CLOCKWISE INTEGRAL
∲	8754	2232	 	CLOCKWISE CONTOUR INTEGRAL
∳	8755	2233	 	ANTICLOCKWISE CONTOUR INTEGRAL
∴	8756	2234	&there4;	THEREFORE
∵	8757	2235	 	BECAUSE
∶	8758	2236	 	RATIO
∷	8759	2237	 	PROPORTION
∸	8760	2238	 	DOT MINUS
∹	8761	2239	 	EXCESS
∺	8762	223A	 	GEOMETRIC PROPORTION
∻	8763	223B	 	HOMOTHETIC
∼	8764	223C	&sim;	TILDE OPERATOR
∽	8765	223D	 	REVERSED TILDE
∾	8766	223E	 	INVERTED LAZY S
∿	8767	223F	 	SINE WAVE
≀	8768	2240	 	WREATH PRODUCT
≁	8769	2241	 	NOT TILDE
≂	8770	2242	 	MINUS TILDE
≃	8771	2243	 	ASYMPTOTICALLY EQUAL TO
≄	8772	2244	 	NOT ASYMPTOTICALLY EQUAL TO
≅	8773	2245	&cong;	APPROXIMATELY EQUAL TO
≆	8774	2246	 	APPROXIMATELY BUT NOT ACTUALLY EQUAL TO
≇	8775	2247	 	NEITHER APPROXIMATELY NOR ACTUALLY EQUAL TO
≈	8776	2248	&asymp;	ALMOST EQUAL TO
≉	8777	2249	 	NOT ALMOST EQUAL TO
≊	8778	224A	 	ALMOST EQUAL OR EQUAL TO
≋	8779	224B	 	TRIPLE TILDE
≌	8780	224C	 	ALL EQUAL TO
≍	8781	224D	 	EQUIVALENT TO
≎	8782	224E	 	GEOMETRICALLY EQUIVALENT TO
≏	8783	224F	 	DIFFERENCE BETWEEN
≐	8784	2250	 	APPROACHES THE LIMIT
≑	8785	2251	 	GEOMETRICALLY EQUAL TO
≒	8786	2252	 	APPROXIMATELY EQUAL TO OR THE IMAGE OF
≓	8787	2253	 	IMAGE OF OR APPROXIMATELY EQUAL TO
≔	8788	2254	 	COLON EQUALS
≕	8789	2255	 	EQUALS COLON
≖	8790	2256	 	RING IN EQUAL TO
≗	8791	2257	 	RING EQUAL TO
≘	8792	2258	 	CORRESPONDS TO
≙	8793	2259	 	ESTIMATES
≚	8794	225A	 	EQUIANGULAR TO
≛	8795	225B	 	STAR EQUALS
≜	8796	225C	 	DELTA EQUAL TO
≝	8797	225D	 	EQUAL TO BY DEFINITION
≞	8798	225E	 	MEASURED BY
≟	8799	225F	 	QUESTIONED EQUAL TO
≠	8800	2260	&ne;	NOT EQUAL TO
≡	8801	2261	&equiv;	IDENTICAL TO
≢	8802	2262	 	NOT IDENTICAL TO
≣	8803	2263	 	STRICTLY EQUIVALENT TO
≤	8804	2264	&le;	LESS-THAN OR EQUAL TO
≥	8805	2265	&ge;	GREATER-THAN OR EQUAL TO
≦	8806	2266	 	LESS-THAN OVER EQUAL TO
≧	8807	2267	 	GREATER-THAN OVER EQUAL TO
≨	8808	2268	 	LESS-THAN BUT NOT EQUAL TO
≩	8809	2269	 	GREATER-THAN BUT NOT EQUAL TO
≪	8810	226A	 	MUCH LESS-THAN
≫	8811	226B	 	MUCH GREATER-THAN
≬	8812	226C	 	BETWEEN
≭	8813	226D	 	NOT EQUIVALENT TO
≮	8814	226E	 	NOT LESS-THAN
≯	8815	226F	 	NOT GREATER-THAN
≰	8816	2270	 	NEITHER LESS-THAN NOR EQUAL TO
≱	8817	2271	 	NEITHER GREATER-THAN NOR EQUAL TO
≲	8818	2272	 	LESS-THAN OR EQUIVALENT TO
≳	8819	2273	 	GREATER-THAN OR EQUIVALENT TO
≴	8820	2274	 	NEITHER LESS-THAN NOR EQUIVALENT TO
≵	8821	2275	 	NEITHER GREATER-THAN NOR EQUIVALENT TO
≶	8822	2276	 	LESS-THAN OR GREATER-THAN
≷	8823	2277	 	GREATER-THAN OR LESS-THAN
≸	8824	2278	 	NEITHER LESS-THAN NOR GREATER-THAN
≹	8825	2279	 	NEITHER GREATER-THAN NOR LESS-THAN
≺	8826	227A	 	PRECEDES
≻	8827	227B	 	SUCCEEDS
≼	8828	227C	 	PRECEDES OR EQUAL TO
≽	8829	227D	 	SUCCEEDS OR EQUAL TO
≾	8830	227E	 	PRECEDES OR EQUIVALENT TO
≿	8831	227F	 	SUCCEEDS OR EQUIVALENT TO
⊀	8832	2280	 	DOES NOT PRECEDE
⊁	8833	2281	 	DOES NOT SUCCEED
⊂	8834	2282	&sub;	SUBSET OF
⊃	8835	2283	&sup;	SUPERSET OF
⊄	8836	2284	&nsub;	NOT A SUBSET OF
⊅	8837	2285	 	NOT A SUPERSET OF
⊆	8838	2286	&sube;	SUBSET OF OR EQUAL TO
⊇	8839	2287	&supe;	SUPERSET OF OR EQUAL TO
⊈	8840	2288	 	NEITHER A SUBSET OF NOR EQUAL TO
⊉	8841	2289	 	NEITHER A SUPERSET OF NOR EQUAL TO
⊊	8842	228A	 	SUBSET OF WITH NOT EQUAL TO
⊋	8843	228B	 	SUPERSET OF WITH NOT EQUAL TO
⊌	8844	228C	 	MULTISET
⊍	8845	228D	 	MULTISET MULTIPLICATION
⊎	8846	228E	 	MULTISET UNION
⊏	8847	228F	 	SQUARE IMAGE OF
⊐	8848	2290	 	SQUARE ORIGINAL OF
⊑	8849	2291	 	SQUARE IMAGE OF OR EQUAL TO
⊒	8850	2292	 	SQUARE ORIGINAL OF OR EQUAL TO
⊓	8851	2293	 	SQUARE CAP
⊔	8852	2294	 	SQUARE CUP
⊕	8853	2295	&oplus;	CIRCLED PLUS
⊖	8854	2296	 	CIRCLED MINUS
⊗	8855	2297	&otimes;	CIRCLED TIMES
⊘	8856	2298	 	CIRCLED DIVISION SLASH
⊙	8857	2299	 	CIRCLED DOT OPERATOR
⊚	8858	229A	 	CIRCLED RING OPERATOR
⊛	8859	229B	 	CIRCLED ASTERISK OPERATOR
⊜	8860	229C	 	CIRCLED EQUALS
⊝	8861	229D	 	CIRCLED DASH
⊞	8862	229E	 	SQUARED PLUS
⊟	8863	229F	 	SQUARED MINUS
⊠	8864	22A0	 	SQUARED TIMES
⊡	8865	22A1	 	SQUARED DOT OPERATOR
⊢	8866	22A2	 	RIGHT TACK
⊣	8867	22A3	 	LEFT TACK
⊤	8868	22A4	 	DOWN TACK
⊥	8869	22A5	&perp;	UP TACK
⊦	8870	22A6	 	ASSERTION
⊧	8871	22A7	 	MODELS
⊨	8872	22A8	 	TRUE
⊩	8873	22A9	 	FORCES
⊪	8874	22AA	 	TRIPLE VERTICAL BAR RIGHT TURNSTILE
⊫	8875	22AB	 	DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE
⊬	8876	22AC	 	DOES NOT PROVE
⊭	8877	22AD	 	NOT TRUE
⊮	8878	22AE	 	DOES NOT FORCE
⊯	8879	22AF	 	NEGATED DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE
⊰	8880	22B0	 	PRECEDES UNDER RELATION
⊱	8881	22B1	 	SUCCEEDS UNDER RELATION
⊲	8882	22B2	 	NORMAL SUBGROUP OF
⊳	8883	22B3	 	CONTAINS AS NORMAL SUBGROUP
⊴	8884	22B4	 	NORMAL SUBGROUP OF OR EQUAL TO
⊵	8885	22B5	 	CONTAINS AS NORMAL SUBGROUP OR EQUAL TO
⊶	8886	22B6	 	ORIGINAL OF
⊷	8887	22B7	 	IMAGE OF
⊸	8888	22B8	 	MULTIMAP
⊹	8889	22B9	 	HERMITIAN CONJUGATE MATRIX
⊺	8890	22BA	 	INTERCALATE
⊻	8891	22BB	 	XOR
⊼	8892	22BC	 	NAND
⊽	8893	22BD	 	NOR
⊾	8894	22BE	 	RIGHT ANGLE WITH ARC
⊿	8895	22BF	 	RIGHT TRIANGLE
⋀	8896	22C0	 	N-ARY LOGICAL AND
⋁	8897	22C1	 	N-ARY LOGICAL OR
⋂	8898	22C2	 	N-ARY INTERSECTION
⋃	8899	22C3	 	N-ARY UNION
⋄	8900	22C4	 	DIAMOND OPERATOR
⋅	8901	22C5	&sdot;	DOT OPERATOR
⋆	8902	22C6	 	STAR OPERATOR
⋇	8903	22C7	 	DIVISION TIMES
⋈	8904	22C8	 	BOWTIE
⋉	8905	22C9	 	LEFT NORMAL FACTOR SEMIDIRECT PRODUCT
⋊	8906	22CA	 	RIGHT NORMAL FACTOR SEMIDIRECT PRODUCT
⋋	8907	22CB	 	LEFT SEMIDIRECT PRODUCT
⋌	8908	22CC	 	RIGHT SEMIDIRECT PRODUCT
⋍	8909	22CD	 	REVERSED TILDE EQUALS
⋎	8910	22CE	 	CURLY LOGICAL OR
⋏	8911	22CF	 	CURLY LOGICAL AND
⋐	8912	22D0	 	DOUBLE SUBSET
⋑	8913	22D1	 	DOUBLE SUPERSET
⋒	8914	22D2	 	DOUBLE INTERSECTION
⋓	8915	22D3	 	DOUBLE UNION
⋔	8916	22D4	 	PITCHFORK
⋕	8917	22D5	 	EQUAL AND PARALLEL TO
⋖	8918	22D6	 	LESS-THAN WITH DOT
⋗	8919	22D7	 	GREATER-THAN WITH DOT
⋘	8920	22D8	 	VERY MUCH LESS-THAN
⋙	8921	22D9	 	VERY MUCH GREATER-THAN
⋚	8922	22DA	 	LESS-THAN EQUAL TO OR GREATER-THAN
⋛	8923	22DB	 	GREATER-THAN EQUAL TO OR LESS-THAN
⋜	8924	22DC	 	EQUAL TO OR LESS-THAN
⋝	8925	22DD	 	EQUAL TO OR GREATER-THAN
⋞	8926	22DE	 	EQUAL TO OR PRECEDES
⋟	8927	22DF	 	EQUAL TO OR SUCCEEDS
⋠	8928	22E0	 	DOES NOT PRECEDE OR EQUAL
⋡	8929	22E1	 	DOES NOT SUCCEED OR EQUAL
⋢	8930	22E2	 	NOT SQUARE IMAGE OF OR EQUAL TO
⋣	8931	22E3	 	NOT SQUARE ORIGINAL OF OR EQUAL TO
⋤	8932	22E4	 	SQUARE IMAGE OF OR NOT EQUAL TO
⋥	8933	22E5	 	SQUARE ORIGINAL OF OR NOT EQUAL TO
⋦	8934	22E6	 	LESS-THAN BUT NOT EQUIVALENT TO
⋧	8935	22E7	 	GREATER-THAN BUT NOT EQUIVALENT TO
⋨	8936	22E8	 	PRECEDES BUT NOT EQUIVALENT TO
⋩	8937	22E9	 	SUCCEEDS BUT NOT EQUIVALENT TO
⋪	8938	22EA	 	NOT NORMAL SUBGROUP OF
⋫	8939	22EB	 	DOES NOT CONTAIN AS NORMAL SUBGROUP
⋬	8940	22EC	 	NOT NORMAL SUBGROUP OF OR EQUAL TO
⋭	8941	22ED	 	DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL
⋮	8942	22EE	 	VERTICAL ELLIPSIS
⋯	8943	22EF	 	MIDLINE HORIZONTAL ELLIPSIS
⋰	8944	22F0	 	UP RIGHT DIAGONAL ELLIPSIS
⋱	8945	22F1	 	DOWN RIGHT DIAGONAL ELLIPSIS
⋲	8946	22F2	 	ELEMENT OF WITH LONG HORIZONTAL STROKE
⋳	8947	22F3	 	ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
⋴	8948	22F4	 	SMALL ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
⋵	8949	22F5	 	ELEMENT OF WITH DOT ABOVE
⋶	8950	22F6	 	ELEMENT OF WITH OVERBAR
⋷	8951	22F7	 	SMALL ELEMENT OF WITH OVERBAR
⋸	8952	22F8	 	ELEMENT OF WITH UNDERBAR
⋹	8953	22F9	 	ELEMENT OF WITH TWO HORIZONTAL STROKES
⋺	8954	22FA	 	CONTAINS WITH LONG HORIZONTAL STROKE
⋻	8955	22FB	 	CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
⋼	8956	22FC	 	SMALL CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
⋽	8957	22FD	 	CONTAINS WITH OVERBAR
⋾	8958	22FE	 	SMALL CONTAINS WITH OVERBAR
⋿	8959	22FF	 	Z NOTATION BAG MEMBERSHIP

❬	10092	276C	 	MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
❭	10093	276D	 	MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
❮	10094	276E	 	HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
❯	10095	276F	 	HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
❰	10096	2770	 	HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
❱	10097	2771	 	HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMEN
*/
}
