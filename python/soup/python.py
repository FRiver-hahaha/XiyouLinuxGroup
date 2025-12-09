import requests as rq
import bs4 as bs

headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
}

def open_url(url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
    }
    # 这里应该使用传入的url参数，而不是固定字符串
    res = rq.get(url, headers=headers)
    return res

def find_movies(res):
    soup = bs.BeautifulSoup(res.text, "html.parser")

    movies = []
    targets = soup.find_all("div", {"class": "hd"})
    for each in targets:
        movies.append(each.a.span.text)

    ranks = []
    targets = soup.find_all("span", {"class": "rating_num"})  # 这里应该是span标签
    for each in targets:
        ranks.append(f"评分：{each.text}")

    messages = []
    targets = soup.find_all("div", {"class": "bd"})
    for each in targets:
        try:
            # 提取导演和演员信息
            text_lines = each.p.text.split('\n')
            if len(text_lines) >= 3:
                messages.append(text_lines[1].strip() + text_lines[2].strip())
        except:
            continue

    result = []
    length = min(len(movies), len(ranks), len(messages))  # 确保长度一致
    for i in range(length):
        # 修正这里的拼接逻辑
        result.append(movies[i] + " " + ranks[i] + " " + messages[i])

    return result

def find_depth(res):
    soup = bs.BeautifulSoup(res.text, "html.parser")
    # 修正获取总页数的逻辑
    next_span = soup.find("span", {"class": "next"})
    if next_span and next_span.previous_sibling and next_span.previous_sibling.previous_sibling:
        depth = next_span.previous_sibling.previous_sibling.text
    else:
        # 如果没有找到分页，默认只有1页
        depth = "1"
    return int(depth)

def main():
    host = "https://movie.douban.com/top250"  # 建议使用https
    res = open_url(host)
    depth = find_depth(res)

    result = []
    for i in range(depth):
        url = host + '?start=' + str(25 * i) + '&filter='
        res = open_url(url)
        result.extend(find_movies(res))

    with open('top250.txt', 'w', encoding='utf-8') as f:  # 修正编码为'utf-8'
        for each in result:
            f.write(each + '\n')
    print(f"成功爬取{len(result)}部电影信息")

if __name__ == '__main__':
    main()