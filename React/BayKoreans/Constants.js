export const midsMap = {
    'drama': '방영 드라마',
    'drama_fin': '종영 드라마',
    'current': '시사|교양',
    'entertain': '연예|오락',
};

export const bookmarkCookieKey = 'BayKoreansBookmarks';

export const defaultBookmarks = [
    ['drama', '도깨비'],
    ['current', '이야기 Y'],
    ['current', '알고 싶다'],
    ['entertain', '순간포착'],
];

export function urlViaCorsProxy(url)
{
    // Cross-origin resource sharing
    //  https://en.wikipedia.org/wiki/Cross-origin_resource_sharing
    // google "cors proxy"
    const corsProxy = 'http://cors-anywhere.herokuapp.com/';
    return corsProxy + url;
}
